/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/color.h"
#include "egt/palette.h"
#include "egt/screen.h"
#include "egt/types.h"
#include "egt/utils.h"
#include <cairo.h>
#include <cassert>
#include <map>

using namespace std;

namespace egt
{
inline namespace v1
{

static Screen* the_screen = nullptr;

Screen*& main_screen()
{
    return the_screen;
}

void Screen::flip(const damage_array& damage)
{
    if (!damage.empty() && index() < m_buffers.size())
    {
        // save the damage to all buffers
        for (auto& b : m_buffers)
            for (const auto& d : damage)
                b.add_damage(d);

        DisplayBuffer& buffer = m_buffers[index()];

        copy_to_buffer(buffer);

        // delete all damage from current buffer
        buffer.damage.clear();
        schedule_flip();
    }
}

void Screen::copy_to_buffer(DisplayBuffer& buffer)
{
    cairo_set_source_surface(buffer.cr.get(), m_surface.get(), 0, 0);
    cairo_set_operator(buffer.cr.get(), CAIRO_OPERATOR_SOURCE);

    for (const auto& rect : buffer.damage)
        cairo_rectangle(buffer.cr.get(), rect.x, rect.y, rect.width, rect.height);

    cairo_fill(buffer.cr.get());
    cairo_surface_flush(buffer.surface.get());
}

void Screen::damage_algorithm(Screen::damage_array& damage, const Rect& rect)
{
    if (rect.empty())
        return;

    // work backwards for a stronger hit chance for existing rectangles
    for (auto i = damage.rbegin(); i != damage.rend(); ++i)
    {
        // exact rectangle already exists; done
        if (*i == rect)
            return;

        // if this rectangle intersects an existing rectangle, then merge
        // the rectangles and re-add the super rectangle
        if (Rect::intersect(*i, rect))
        {
            Rect super(Rect::merge(*i, rect));
            damage.erase(std::next(i).base());
            Screen::damage_algorithm(damage, super);
            return;
        }
    }

    // if we get here, no intersect found so add it
    damage.emplace_back(rect);
}

void Screen::init(void** ptr, uint32_t count, const Size& size, pixel_format format)
{
    m_size = size;

    cairo_format_t f = detail::cairo_format(format);
    if (f == CAIRO_FORMAT_INVALID)
        f = CAIRO_FORMAT_ARGB32;

    m_buffers.clear();

    for (uint32_t x = 0; x < count; x++)
    {
        m_buffers.emplace_back(
            cairo_image_surface_create_for_data(reinterpret_cast<unsigned char*>(ptr[x]),
                                                f,
                                                size.width, size.height,
                                                cairo_format_stride_for_width(f, size.width)));

        m_buffers.back().damage.emplace_back(Point(), size);
    }

    m_surface = shared_cairo_surface_t(cairo_image_surface_create(f, size.width, size.height),
                                       cairo_surface_destroy);

    assert(m_surface.get());

    m_cr = shared_cairo_t(cairo_create(m_surface.get()), cairo_destroy);
    assert(m_cr);

    cairo_font_options_t* cfo = cairo_font_options_create();
    cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_FAST);
    cairo_font_options_set_hint_style(cfo, CAIRO_HINT_STYLE_SLIGHT);
    cairo_set_font_options(m_cr.get(), cfo);
    cairo_font_options_destroy(cfo);
    cairo_set_antialias(m_cr.get(), CAIRO_ANTIALIAS_FAST);

    if (!the_screen)
        the_screen = this;
}

}
}
