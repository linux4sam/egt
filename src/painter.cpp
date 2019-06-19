/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/image.h"
#include "egt/painter.h"
#include <cairo.h>

namespace egt
{
inline namespace v1
{

Painter::Painter(shared_cairo_t cr) noexcept
    : m_cr(cr)
{
}

void Painter::save()
{
    cairo_save(m_cr.get());
}

void Painter::restore()
{
    cairo_restore(m_cr.get());
}

Painter& Painter::set(const Color& color)
{
    cairo_set_source_rgba(m_cr.get(),
                          color.redf(),
                          color.greenf(),
                          color.bluef(),
                          color.alphaf());

    return *this;
}

Painter& Painter::set(const Font& font)
{
    cairo_set_scaled_font(m_cr.get(), font.scaled_font());

    return *this;
}

Painter& Painter::set_line_width(float width)
{
    cairo_set_line_width(m_cr.get(), width);

    return *this;
}

Painter& Painter::draw(const Image& image)
{
    assert(!image.empty());
    if (image.empty())
        return *this;

    AutoSaveRestore sr(*this);

    if (!cairo_has_current_point(m_cr.get()))
        return *this;

    double x, y;
    cairo_get_current_point(m_cr.get(), &x, &y);
    cairo_set_source_surface(m_cr.get(), image.surface().get(), x, y);
    cairo_rectangle(m_cr.get(), x, y, image.size().width, image.size().height);

    /// @todo no fill here
    fill();

    return *this;
}

Painter& Painter::draw(const Rect& rect, const Image& image)
{
    double x, y;

    assert(!image.empty());
    if (image.empty())
        return *this;

    if (!cairo_has_current_point(m_cr.get()))
        return *this;

    cairo_get_current_point(m_cr.get(), &x, &y);
    cairo_set_source_surface(m_cr.get(), image.surface().get(),
                             x - rect.x, y - rect.y);
    cairo_rectangle(m_cr.get(), x, y, rect.width, rect.height);

    /// @todo no fill here
    fill();

    return *this;
}

Painter& Painter::draw(const std::string& str, bool difference)
{
    if (str.empty())
        return *this;

    if (!cairo_has_current_point(m_cr.get()))
        return *this;

    double x, y;
    cairo_text_extents_t textext;

    cairo_text_extents(m_cr.get(), str.c_str(), &textext);

    AutoSaveRestore sr(*this);

    cairo_get_current_point(m_cr.get(), &x, &y);
    cairo_move_to(m_cr.get(), x - textext.x_bearing,
                  y - textext.y_bearing);
    if (difference)
        cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_DIFFERENCE);
    cairo_show_text(m_cr.get(), str.c_str());
    cairo_stroke(m_cr.get());

    return *this;
}

Size Painter::text_size(const std::string& text)
{
    cairo_text_extents_t textext;
    cairo_text_extents(m_cr.get(), text.c_str(), &textext);
    return Size(textext.width, textext.height);
}

Size Painter::font_size(const std::string& text)
{
    cairo_font_extents_t fe;
    cairo_font_extents(m_cr.get(), &fe);
    return Size(text_size(text).width, fe.height);
}

Painter& Painter::clip()
{
    cairo_clip(m_cr.get());

    return *this;
}

Painter& Painter::fill()
{
    cairo_fill(m_cr.get());

    return *this;
}

Painter& Painter::paint()
{
    cairo_paint(m_cr.get());

    return *this;
}

Painter& Painter::stroke()
{
    cairo_stroke(m_cr.get());

    return *this;
}

}
}
