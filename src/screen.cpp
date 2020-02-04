/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/color.h"
#include "egt/detail/dump.h"
#include "egt/palette.h"
#include "egt/screen.h"
#include "egt/types.h"
#include <cairo.h>
#include <cassert>
#include <cstring>
#include <map>

#ifdef HAVE_SIMD
#include "Simd/SimdLib.hpp"
#endif

namespace egt
{
inline namespace v1
{

Screen::Screen()
{
    if (getenv("EGT_SCREEN_ASYNC_FLIP"))
    {
        if (strlen(getenv("EGT_SCREEN_ASYNC_FLIP")))
            m_async = true;
    }
}

void Screen::flip(const DamageArray& damage)
{
    if (!damage.empty() && index() < m_buffers.size())
    {
        // save the damage to all buffers
        for (auto& b : m_buffers)
            for (const auto& d : damage)
                b.add_damage(d);

        detail::code_timer(false, "copy_to_buffer: ", [&]()
        {
            ScreenBuffer& buffer = m_buffers[index()];
            copy_to_buffer(buffer);
            // delete all damage from current buffer
            buffer.damage.clear();
        });

        schedule_flip();
    }
}

#ifdef HAVE_SIMD

using View = Simd::View<Simd::Allocator>;

static inline View::Format simd_format(cairo_format_t format)
{
    static const std::map<cairo_format_t, View::Format> simd_formats =
    {
        {CAIRO_FORMAT_RGB16_565, View::Int16},
        {CAIRO_FORMAT_ARGB32, View::Int32},
        {CAIRO_FORMAT_RGB24, View::Int32},
    };

    const auto i = simd_formats.find(format);
    if (i != simd_formats.end())
        return i->second;

    return View::None;
}

static void simd_copy(cairo_surface_t* src_surface,
                      cairo_surface_t* dst_surface,
                      const Screen::DamageArray& damage)
{
    cairo_surface_flush(src_surface);

    auto src = cairo_image_surface_get_data(src_surface);
    auto dst = cairo_image_surface_get_data(dst_surface);

    assert(src);
    assert(dst);

    auto src_format = cairo_image_surface_get_format(src_surface);
    auto dst_format = cairo_image_surface_get_format(dst_surface);

    assert(src_format == dst_format);

    auto src_width = cairo_image_surface_get_width(src_surface);
    auto src_height = cairo_image_surface_get_height(src_surface);

    auto dst_width = cairo_image_surface_get_width(dst_surface);
    auto dst_height = cairo_image_surface_get_height(dst_surface);

    assert(src_width == dst_width);
    assert(src_height == dst_height);

    View srcview(src_width, src_height,
                 cairo_format_stride_for_width(src_format, src_width),
                 simd_format(src_format), src);
    View dstview(dst_width, dst_height,
                 cairo_format_stride_for_width(dst_format, dst_width),
                 simd_format(dst_format), dst);

    if (damage.size() == 1 &&
        damage[0] == Rect(Point(), Size(src_width, src_height)))
    {
        memcpy(dst, src,
               src_width * dst_height * (src_format == CAIRO_FORMAT_RGB16_565 ? 2 : 4));
    }
    else
    {
        for (const auto& rect : damage)
        {
            Simd::Copy(srcview.Region(rect.x(), rect.y(), rect.right(), rect.bottom()),
                       dstview.Region(rect.x(), rect.y(), rect.right(), rect.bottom()).Ref());
        }
    }

    cairo_surface_mark_dirty(dst_surface);
}

void Screen::copy_to_buffer(ScreenBuffer& buffer)
{
    simd_copy(m_surface.get(), buffer.surface.get(), buffer.damage);
}
#else
void Screen::copy_to_buffer(ScreenBuffer& buffer)
{
    copy_to_buffer_software(buffer);
}
#endif

void Screen::copy_to_buffer_software(ScreenBuffer& buffer)
{
    // create a new context for each frame
    unique_cairo_t cr(cairo_create(buffer.surface.get()));

    cairo_set_source_surface(cr.get(), m_surface.get(), 0, 0);
    cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);

    for (const auto& rect : buffer.damage)
        cairo_rectangle(cr.get(), rect.x(), rect.y(), rect.width(), rect.height());

    cairo_fill(cr.get());
    cairo_surface_flush(buffer.surface.get());
}

void Screen::damage_algorithm(Screen::DamageArray& damage, Rect rect)
{
    if (rect.empty())
        return;

    // work backwards for a stronger hit chance for existing rectangles
    for (auto i = damage.rbegin(); i != damage.rend();)
    {
        // exact rectangle already exists; done
        if (*i == rect)
            return;

        // if this rectangle intersects an existing rectangle, then merge
        // the rectangles and start over
        if (rect.intersect(*i))
        {
            rect = Rect::merge(*i, rect);
            damage.erase(std::next(i).base());
            i = damage.rbegin();
            continue;
        }

        ++i;
    }

    // if we get here, no intersect found so add it
    damage.emplace_back(rect);
}

static inline bool no_composition_buffer()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_NO_COMPOSITION_BUFFER"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

void Screen::init(void** ptr, uint32_t count, const Size& size, PixelFormat format)
{
    m_size = size;

    cairo_format_t f = detail::cairo_format(format);
    if (f == CAIRO_FORMAT_INVALID)
        f = CAIRO_FORMAT_ARGB32;

    m_buffers.clear();

    if (count == 1 && no_composition_buffer())
    {
        m_surface = shared_cairo_surface_t(
                        cairo_image_surface_create_for_data(reinterpret_cast<unsigned char*>(ptr[0]),
                                f,
                                size.width(), size.height(),
                                cairo_format_stride_for_width(f, size.width())),
                        cairo_surface_destroy);
    }
    else
    {
        for (uint32_t x = 0; x < count; x++)
        {
            m_buffers.emplace_back(
                cairo_image_surface_create_for_data(reinterpret_cast<unsigned char*>(ptr[x]),
                                                    f,
                                                    size.width(), size.height(),
                                                    cairo_format_stride_for_width(f, size.width())));

            m_buffers.back().damage.emplace_back(Point(), size);
        }

        m_surface = shared_cairo_surface_t(cairo_image_surface_create(f, size.width(), size.height()),
                                           cairo_surface_destroy);
    }

    assert(m_surface.get());

    m_cr = shared_cairo_t(cairo_create(m_surface.get()), cairo_destroy);
    assert(m_cr);

    m_format = format;
}

void Screen::low_fidelity()
{
    // font
    cairo_font_options_t* cfo = cairo_font_options_create();
    cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_FAST);
    cairo_font_options_set_hint_style(cfo, CAIRO_HINT_STYLE_NONE);
    cairo_set_font_options(m_cr.get(), cfo);
    cairo_font_options_destroy(cfo);

    // shapes
    cairo_set_antialias(m_cr.get(), CAIRO_ANTIALIAS_FAST);
}

void Screen::high_fidelity()
{
    // font
    cairo_font_options_t* cfo = cairo_font_options_create();
    cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_GOOD);
    cairo_font_options_set_hint_style(cfo, CAIRO_HINT_STYLE_MEDIUM);
    cairo_set_font_options(m_cr.get(), cfo);
    cairo_font_options_destroy(cfo);

    // shapes
    cairo_set_antialias(m_cr.get(), CAIRO_ANTIALIAS_GOOD);
}


size_t Screen::max_brightness() const
{
    return 100;
}

size_t Screen::brightness() const
{
    return 100;
}

void Screen::brightness(size_t brightness)
{
}

Screen::~Screen() = default;

}
}
