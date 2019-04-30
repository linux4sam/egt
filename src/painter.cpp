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

Painter::Painter(shared_cairo_t cr)
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
    cairo_font_weight_t weight = CAIRO_FONT_WEIGHT_NORMAL;
    switch (font.weight())
    {
    case Font::weightid::normal:
        break;
    case Font::weightid::bold:
        weight = CAIRO_FONT_WEIGHT_BOLD;
        break;
    }

    cairo_font_slant_t slant = CAIRO_FONT_SLANT_NORMAL;
    switch (font.slant())
    {
    case Font::slantid::normal:
        break;
    case Font::slantid::italic:
        slant = CAIRO_FONT_SLANT_ITALIC;
        break;
    case Font::slantid::oblique:
        slant = CAIRO_FONT_SLANT_OBLIQUE;
        break;
    }

    cairo_select_font_face(m_cr.get(),
                           font.face().c_str(),
                           slant,
                           weight);
    cairo_set_font_size(m_cr.get(), font.size());

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
    cairo_rectangle(m_cr.get(), x, y, image.size().w, image.size().h);

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
    cairo_rectangle(m_cr.get(), x, y, rect.w, rect.h);

    /// @todo no fill here
    fill();

    return *this;
}

Painter& Painter::draw(const std::string& str, bool difference)
{
    if (str.empty())
        return *this;

    double x, y;
    cairo_font_extents_t fe;
    cairo_text_extents_t textext;

    cairo_font_extents(m_cr.get(), &fe);
    cairo_text_extents(m_cr.get(), str.c_str(), &textext);

    if (!cairo_has_current_point(m_cr.get()))
        return *this;

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
    return Size(text_size(text).w, fe.height);
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

/* https://people.freedesktop.org/~joonas/shadow.c */

/* Returns a surface with content COLOR_ALPHA which is the same size
 * as the passed surface. */
static cairo_surface_t* surface_create_similar(cairo_surface_t* surface)
{
    cairo_t* cr = cairo_create(surface);
    cairo_surface_t* similar;
    cairo_push_group(cr);
    similar = cairo_get_group_target(cr);
    cairo_surface_reference(similar);
    cairo_destroy(cr);
    return similar;
}

/* Paint the given surface with a drop shadow to the context cr. */
void Painter::paint_surface_with_drop_shadow(cairo_surface_t* source_surface,
        int shadow_offset,
        double shadow_alpha,
        double tint_alpha,
        /*int srx,
        int srcy,
        int width,
        int height,*/
        int dstx,
        int dsty)
{
    /* A temporary surface the size of the source surface.  */
    cairo_surface_t* shadow_surface = surface_create_similar(source_surface);

    /* Draw the shadow to the shadow surface. */
    {
        cairo_t* cr = cairo_create(shadow_surface);
        if (tint_alpha < 1.0)
        {
            /* Draw the shadow image with the desired transparency. */
            cairo_set_source_surface(cr, source_surface, 0, 0);
            cairo_paint_with_alpha(cr, shadow_alpha);

            /* Darken the shadow by tinting it with black. The
             * tint_alpha determines how much black to place on top
             * of the shadow image. */
            cairo_set_operator(cr, CAIRO_OPERATOR_ATOP);
            cairo_set_source_rgba(cr, 0, 0, 0, tint_alpha);
            cairo_paint(cr);
        }
        else
        {
            /* Identical to the above when tint_alpha = 1.0, but
             * ostensibly faster. */
            cairo_pattern_t* shadow_mask =
                cairo_pattern_create_for_surface(source_surface);
            cairo_set_source_rgba(cr, 0, 0, 0, shadow_alpha);
            cairo_mask(cr, shadow_mask);
            cairo_pattern_destroy(shadow_mask);
        }
        cairo_destroy(cr);
    }

    /* Paint the shadow surface to cr. */
    cairo_save(m_cr.get());
    {
        cairo_translate(m_cr.get(), shadow_offset, shadow_offset);
        cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_OVER);
        cairo_set_source_surface(m_cr.get(), shadow_surface, dstx, dsty);
        cairo_paint(m_cr.get());
    }
    cairo_restore(m_cr.get());
    cairo_surface_destroy(shadow_surface);

    /* Paint the image itself to cr. */
    cairo_set_source_surface(m_cr.get(), source_surface, dstx, dsty);
    cairo_paint(m_cr.get());
}

}
}
