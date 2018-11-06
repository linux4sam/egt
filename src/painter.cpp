/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "egt/painter.h"
#include "egt/widget.h"

namespace egt
{

    Painter::Painter(shared_cairo_t cr)
        : m_cr(cr)
    {
        begin();
    }

    Painter::~Painter()
    {
        end();
    }

    void Painter::begin()
    {

    }

    void Painter::end()
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

    Painter& Painter::set_color(const Color& color)
    {
        cairo_set_source_rgba(m_cr.get(),
                              color.redf(),
                              color.greenf(),
                              color.bluef(),
                              color.alphaf());

        return *this;
    }

    Painter& Painter::rectangle(const Rect& rect)
    {
        cairo_rectangle(m_cr.get(),
                        rect.x,
                        rect.y,
                        rect.w,
                        rect.h);

        return *this;
    }

    Painter& Painter::draw_fill(const Rect& rect)
    {
        cairo_rectangle(m_cr.get(),
                        rect.x,
                        rect.y,
                        rect.w,
                        rect.h);
        cairo_fill(m_cr.get());

        return *this;
    }

    Painter& Painter::set_line_width(float width)
    {
        cairo_set_line_width(m_cr.get(), width);

        return *this;
    }

    Painter& Painter::line(const Point& start, const Point& end)
    {
        cairo_move_to(m_cr.get(), start.x, start.y);
        cairo_line_to(m_cr.get(), end.x, end.y);

        return *this;
    }

    Painter& Painter::line(const Point& end)
    {
        cairo_line_to(m_cr.get(), end.x, end.y);

        return *this;
    }

    Painter& Painter::draw_image(const Point& point, shared_cairo_surface_t surface, bool bw)
    {
        double w = cairo_image_surface_get_width(surface.get());
        double h = cairo_image_surface_get_height(surface.get());
        cairo_set_source_surface(m_cr.get(), surface.get(), point.x, point.y);
        cairo_rectangle(m_cr.get(), point.x, point.y, w, h);
        cairo_fill(m_cr.get());

        if (bw)
        {
            AutoSaveRestore sr(*this);

            cairo_set_source_rgb(m_cr.get(), 0, 0, 0);
            cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_HSL_COLOR);
            cairo_mask_surface(m_cr.get(),
                               surface.get(),
                               point.x,
                               point.y);
        }

        return *this;
    }

    Painter& Painter::draw_image(const Rect& rect, const Point& point, shared_cairo_surface_t surface)
    {
        cairo_set_source_surface(m_cr.get(), surface.get(), point.x - rect.x, point.y - rect.y);
        cairo_rectangle(m_cr.get(), point.x, point.y, rect.w, rect.h);
        cairo_fill(m_cr.get());

        return *this;
    }

    Painter& Painter::draw_arc(const Point& point, float radius, float angle1, float angle2)
    {
        cairo_arc(m_cr.get(), point.x, point.y, radius, angle1, angle2);
        cairo_stroke(m_cr.get());

        return *this;
    }

    Painter& Painter::set_font(const Font& font)
    {
        cairo_font_weight_t weight = CAIRO_FONT_WEIGHT_NORMAL;
        switch (font.weight())
        {
        case Font::weightid::NORMAL:
            break;
        case Font::weightid::BOLD:
            weight = CAIRO_FONT_WEIGHT_BOLD;
            break;
        }

        cairo_font_slant_t slant = CAIRO_FONT_SLANT_NORMAL;
        switch (font.slant())
        {
        case Font::slantid::NORMAL:
            break;
        case Font::slantid::ITALIC:
            slant = CAIRO_FONT_SLANT_ITALIC;
            break;
        case Font::slantid::OBLIQUE:
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

    Rect Painter::draw_text(const Rect& rect, const std::string& str, alignmask align, int standoff)
    {
        cairo_text_extents_t textext;
        cairo_text_extents(m_cr.get(), str.c_str(), &textext);

        Rect target = Widget::align_algorithm(Size(textext.width, textext.height),
                                              rect,
                                              align,
                                              standoff);

        cairo_move_to(m_cr.get(), target.x + textext.x_bearing,
                      target.y - textext.y_bearing);
        cairo_show_text(m_cr.get(), str.c_str());
        cairo_stroke(m_cr.get());

        Rect bounding = target;
        bounding.h = textext.height;
        bounding.w = textext.width;
        return bounding;
    }

    Rect Painter::draw_text(const std::string& text, const Rect& rect,
                            const Color& color, alignmask align, int margin,
                            const Font& font)
    {
        AutoSaveRestore sr(*this);

        set_font(font);
        set_color(color);
        return draw_text(rect, text, align, margin);
    }

    Size Painter::text_size(const std::string& text)
    {
        cairo_text_extents_t textext;
        cairo_text_extents(m_cr.get(), text.c_str(), &textext);
        return Size(textext.width, textext.height);
    }

    Painter& Painter::clip()
    {
        cairo_clip(m_cr.get());

        return *this;
    }

    Painter& Painter::draw_basic_box(const Rect& rect, const Color& border, const Color& bg)
    {
        AutoSaveRestore sr(*this);

        double rx = rect.x,
               ry = rect.y,
               width = rect.w,
               height = rect.h,
               aspect = 1.0,
               corner_radius = 5.0;

        if (rect.w <= 10)
            corner_radius = rect.w / 2;

        if (rect.h <= 10)
            corner_radius = rect.h / 2;

        double radius = corner_radius / aspect;
        double degrees = M_PI / 180.0;

        cairo_new_sub_path(m_cr.get());
        cairo_arc(m_cr.get(), rx + width - radius, ry + radius, radius, -90. * degrees, 0. * degrees);
        cairo_arc(m_cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90. * degrees);
        cairo_arc(m_cr.get(), rx + radius, ry + height - radius, radius, 90. * degrees, 180. * degrees);
        cairo_arc(m_cr.get(), rx + radius, ry + radius, radius, 180. * degrees, 270. * degrees);
        cairo_close_path(m_cr.get());

        cairo_set_source_rgba(m_cr.get(),
                              bg.redf(),
                              bg.greenf(),
                              bg.bluef(),
                              bg.alphaf());
        cairo_fill_preserve(m_cr.get());

        cairo_set_source_rgba(m_cr.get(),
                              border.redf(),
                              border.greenf(),
                              border.bluef(),
                              border.alphaf());
        cairo_set_line_width(m_cr.get(), 1.0);
        cairo_stroke(m_cr.get());

        return *this;
    }

#if 0
    Painter& Painter::draw_gradient_box(const Rect& rect, const Color& border,
                                        bool active, const Color& color)
    {
        AutoSaveRestore sr(*this);

        double rx = rect.x,
               ry = rect.y,
               width = rect.w,
               height = rect.h,
               aspect = 1.0,
               corner_radius = 50.0 / 10.0;

        if (rect.w <= 10)
            corner_radius = rect.w / 2;

        if (rect.h <= 10)
            corner_radius = rect.h / 2;

        double radius = corner_radius / aspect;
        double degrees = M_PI / 180.0;

        cairo_new_sub_path(m_cr.get());
        cairo_arc(m_cr.get(), rx + width - radius, ry + radius, radius, -90. * degrees, 0. * degrees);
        cairo_arc(m_cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90. * degrees);
        cairo_arc(m_cr.get(), rx + radius, ry + height - radius, radius, 90. * degrees, 180. * degrees);
        cairo_arc(m_cr.get(), rx + radius, ry + radius, radius, 180. * degrees, 270. * degrees);
        cairo_close_path(m_cr.get());

        auto pat = shared_cairo_pattern_t(cairo_pattern_create_linear(rx + width / 2, ry,
                                          rx + width / 2, ry + height),
                                          cairo_pattern_destroy);

        if (!active)
        {
            Color step = color;
            cairo_pattern_add_color_stop_rgb(pat.get(), 0, step.redf(), step.greenf(), step.bluef());
            step = color.tint(.9);
            cairo_pattern_add_color_stop_rgb(pat.get(), 0.43, step.redf(), step.greenf(), step.bluef());
            step = color.tint(.82);
            cairo_pattern_add_color_stop_rgb(pat.get(), 0.5, step.redf(), step.greenf(), step.bluef());
            step = color.tint(.95);
            cairo_pattern_add_color_stop_rgb(pat.get(), 1.0, step.redf(), step.greenf(), step.bluef());
        }
        else
        {
            Color step = color.tint(.95);
            cairo_pattern_add_color_stop_rgb(pat.get(), 1, step.redf(), step.greenf(), step.bluef());
            step = color;
            cairo_pattern_add_color_stop_rgb(pat.get(), 0.5, step.redf(), step.greenf(), step.bluef());
            step = color.tint(.95);
            cairo_pattern_add_color_stop_rgb(pat.get(), 0, step.redf(), step.greenf(), step.bluef());
        }

        cairo_set_source(m_cr.get(), pat.get());
        cairo_fill_preserve(m_cr.get());

        cairo_set_source_rgba(m_cr.get(),
                              border.redf(),
                              border.greenf(),
                              border.bluef(),
                              border.alphaf());
        cairo_set_line_width(m_cr.get(), 1.0);
        cairo_stroke(m_cr.get());

        return *this;
    }
#else
    Painter& Painter::draw_gradient_box(const Rect& rect, const Color& border,
                                        bool active, const Color& color)
    {
        AutoSaveRestore sr(*this);

        double rx = rect.x,
               ry = rect.y,
               width = rect.w,
               height = rect.h,
               aspect = 1.0,
               corner_radius = 50.0 / 10.0;

        if (rect.w <= 10)
            corner_radius = rect.w / 2;

        if (rect.h <= 10)
            corner_radius = rect.h / 2;

        double radius = corner_radius / aspect;
        double degrees = M_PI / 180.0;

        cairo_new_sub_path(m_cr.get());
        cairo_arc(m_cr.get(), rx + width - radius, ry + radius, radius, -90. * degrees, 0. * degrees);
        cairo_arc(m_cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90. * degrees);
        cairo_arc(m_cr.get(), rx + radius, ry + height - radius, radius, 90. * degrees, 180. * degrees);
        cairo_arc(m_cr.get(), rx + radius, ry + radius, radius, 180. * degrees, 270. * degrees);
        cairo_close_path(m_cr.get());

        auto pat = shared_cairo_pattern_t(cairo_pattern_create_linear(rx + width / 2, ry,
                                          rx + width / 2, ry + height),
                                          cairo_pattern_destroy);

        if (!active)
        {
#if 0
            Color step = color;
            cairo_pattern_add_color_stop_rgb(pat.get(), 0, step.redf(), step.greenf(), step.bluef());
            step = color.tint(.5);
            cairo_pattern_add_color_stop_rgb(pat.get(), 0.43, step.redf(), step.greenf(), step.bluef());
            step = color.tint(.9);
            cairo_pattern_add_color_stop_rgb(pat.get(), 0.5, step.redf(), step.greenf(), step.bluef());
            step = color.tint(.9);
            cairo_pattern_add_color_stop_rgb(pat.get(), 1.0, step.redf(), step.greenf(), step.bluef());
#endif
#if 1
            Color step = color;
            cairo_pattern_add_color_stop_rgb(pat.get(), 0, step.redf(), step.greenf(), step.bluef());
            step = color.shade(.1);
            cairo_pattern_add_color_stop_rgb(pat.get(), 0.43, step.redf(), step.greenf(), step.bluef());
            step = color.shade(.15);
            cairo_pattern_add_color_stop_rgb(pat.get(), 0.5, step.redf(), step.greenf(), step.bluef());
            step = color.shade(.18);
            cairo_pattern_add_color_stop_rgb(pat.get(), 1.0, step.redf(), step.greenf(), step.bluef());
#endif
        }
        else
        {
            Color step = color.tint(.95);
            cairo_pattern_add_color_stop_rgb(pat.get(), 1, step.redf(), step.greenf(), step.bluef());
            step = color;
            cairo_pattern_add_color_stop_rgb(pat.get(), 0.5, step.redf(), step.greenf(), step.bluef());
            step = color.tint(.95);
            cairo_pattern_add_color_stop_rgb(pat.get(), 0, step.redf(), step.greenf(), step.bluef());
        }

        cairo_set_source(m_cr.get(), pat.get());
        cairo_fill_preserve(m_cr.get());

        cairo_set_source_rgba(m_cr.get(),
                              border.redf(),
                              border.greenf(),
                              border.bluef(),
                              border.alphaf());
        cairo_set_line_width(m_cr.get(), 1.0);
        cairo_stroke(m_cr.get());

        return *this;
    }
#endif

    Painter& Painter::draw_image(shared_cairo_surface_t image,
                                 const Rect& dest,
                                 alignmask align, int margin, bool bw)
    {
        auto width = cairo_image_surface_get_width(image.get());
        auto height = cairo_image_surface_get_height(image.get());

        Rect target = Widget::align_algorithm(Size(width, height), dest, align, margin);

        if (true)
            draw_image(target.point(), image, bw);
        else
            paint_surface_with_drop_shadow(
                image.get(),
                5,
                0.2,
                0.4,
                /*target.x,
                  target.y,
                  width,
                  height,*/
                target.x,
                target.y);

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
