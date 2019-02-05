/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/color.h"
#include "egt/screen.h"
#include "egt/utils.h"
#include <cassert>
#include <drm_fourcc.h>
#include <linux/fb.h>
#include <map>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{
static const map<pixel_format, uint32_t> drm_formats =
{
    {pixel_format::rgb565, DRM_FORMAT_RGB565},
    {pixel_format::argb8888, DRM_FORMAT_ARGB8888},
    {pixel_format::xrgb8888, DRM_FORMAT_XRGB8888},
    {pixel_format::yuyv, DRM_FORMAT_YUYV},
    {pixel_format::yuv420, DRM_FORMAT_YUV420},
};

static const map<pixel_format, cairo_format_t> cairo_formats =
{
    {pixel_format::rgb565, CAIRO_FORMAT_RGB16_565},
    {pixel_format::argb8888, CAIRO_FORMAT_ARGB32},
    {pixel_format::xrgb8888, CAIRO_FORMAT_RGB24},
};

cairo_format_t cairo_format(pixel_format format)
{
    auto i = cairo_formats.find(format);
    if (i != cairo_formats.end())
        return i->second;

    return CAIRO_FORMAT_INVALID;
}

uint32_t drm_format(pixel_format format)
{
    auto i = drm_formats.find(format);
    if (i != drm_formats.end())
        return i->second;

    return 0;
}

pixel_format egt_format(uint32_t format)
{
    for (auto& i : drm_formats)
        // cppcheck-suppress useStlAlgorithm
        if (i.second == format)
            return i.first;

    return pixel_format::invalid;
}

pixel_format egt_format(cairo_format_t format)
{
    for (auto& i : cairo_formats)
        // cppcheck-suppress useStlAlgorithm
        if (i.second == format)
            return i.first;

    return pixel_format::invalid;
}
}

static IScreen* the_screen = nullptr;

IScreen*& main_screen()
{
    return the_screen;
}

IScreen::IScreen()
{
}

void IScreen::flip(const damage_array& damage)
{
    static int envset = -1;
    if (envset < 0)
        envset = !!getenv("EGT_GREENSCREEN");

    if (!damage.empty() && index() < m_buffers.size())
    {
        damage_array olddamage = m_buffers[index()].damage;

        // save the damage to all buffers
        for (auto& b : m_buffers)
            for (const auto& d : damage)
                b.add_damage(d);

        DisplayBuffer& buffer = m_buffers[index()];

        if (envset)
            copy_to_buffer_greenscreen(buffer, olddamage);
        else
            copy_to_buffer(buffer);

        // delete all damage from current buffer
        buffer.damage.clear();
        schedule_flip();
    }
}

/**
 * @todo greenscreen is broken - does not cover all cases and getting it to
 * work with flipping is difficult.  Should consider going to a single
 * buffer for greenscreen.
 */
void IScreen::copy_to_buffer_greenscreen(DisplayBuffer& buffer,
        const damage_array& olddamage)
{
    cairo_set_source_surface(buffer.cr.get(), m_surface.get(), 0, 0);
    cairo_set_operator(buffer.cr.get(), CAIRO_OPERATOR_SOURCE);

    for (const auto& d : buffer.damage)
        cairo_rectangle(buffer.cr.get(), d.x, d.y, d.w, d.h);

    cairo_fill(buffer.cr.get());

    Color color = Color::GREEN;
    cairo_set_source_rgb(buffer.cr.get(), color.redf(),
                         color.greenf(), color.bluef());
    cairo_set_line_width(buffer.cr.get(), 4.0);

    for (const auto& d : buffer.damage)
        if (find(olddamage.begin(), olddamage.end(), d) != olddamage.end())
            cairo_rectangle(buffer.cr.get(), d.x, d.y, d.w, d.h);
    //cairo_rectangle(buffer.cr.get(), d.x + 2, d.y + 2, d.w - 4, d.h - 4);

    cairo_stroke(buffer.cr.get());

    cairo_surface_flush(buffer.surface.get());
}

void IScreen::copy_to_buffer(DisplayBuffer& buffer)
{
    cairo_set_source_surface(buffer.cr.get(), m_surface.get(), 0, 0);
    cairo_set_operator(buffer.cr.get(), CAIRO_OPERATOR_SOURCE);

    for (const auto& rect : buffer.damage)
        cairo_rectangle(buffer.cr.get(), rect.x, rect.y, rect.w, rect.h);

    cairo_fill(buffer.cr.get());
    cairo_surface_flush(buffer.surface.get());
}

IScreen::~IScreen()
{
}

void IScreen::damage_algorithm(IScreen::damage_array& damage, const Rect& rect)
{
    for (auto i = damage.begin(); i != damage.end(); ++i)
    {
        // exact rectangle already exists; done
        if (*i == rect)
            return;

        // if this rectangle intersects an existing rectangle, then merge
        // the rectangles and re-add the super rectangle
        if (Rect::intersect(*i, rect))
        {
            Rect super(Rect::merge(*i, rect));
#if 0
            /*
             * If the area of the two rectangles minus their
             * intersection area is smaller than the area of the super
             * rectangle, then don't merge
             */
            Rect intersect(Rect::intersection(*i, rect));
            if (((*i).area() + rect.area() - intersect.area()) < super.area())
            {
                break;
            }
#endif
            damage.erase(i);
            IScreen::damage_algorithm(damage, super);
            return;
        }
    }

    // if we get here, no intersect found so add it
    damage.push_back(rect);
}

void IScreen::init(void** ptr, uint32_t count, int w, int h, pixel_format format)
{
    m_size = Size(w, h);

    cairo_format_t f = detail::cairo_format(format);
    if (f == CAIRO_FORMAT_INVALID)
        f = CAIRO_FORMAT_ARGB32;

    for (uint32_t x = 0; x < count; x++)
    {
        DisplayBuffer buffer;
        buffer.surface = shared_cairo_surface_t(
                             cairo_image_surface_create_for_data((unsigned char*)ptr[x],
                                     f,
                                     w, h,
                                     cairo_format_stride_for_width(f, w)),
                             cairo_surface_destroy);
        assert(buffer.surface);

        buffer.cr = shared_cairo_t(cairo_create(buffer.surface.get()), cairo_destroy);
        assert(buffer.cr);

        buffer.damage.emplace_back(0, 0, w, h);

        m_buffers.push_back(buffer);
    }

    m_surface = shared_cairo_surface_t(cairo_image_surface_create(f, w, h),
                                       cairo_surface_destroy);

    assert(m_surface.get());

    m_cr = shared_cairo_t(cairo_create(m_surface.get()), cairo_destroy);
    assert(m_cr);

#if 1
    cairo_font_options_t* cfo = cairo_font_options_create();
    cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_FAST);
    cairo_font_options_set_hint_style(cfo, CAIRO_HINT_STYLE_SLIGHT);
    cairo_set_font_options(m_cr.get(), cfo);
    cairo_font_options_destroy(cfo);

    cairo_set_antialias(m_cr.get(), CAIRO_ANTIALIAS_FAST);
    //cairo_set_tolerance(m_cr.get(), 1.0);
#endif

    if (!the_screen)
        the_screen = this;
}

}
}
