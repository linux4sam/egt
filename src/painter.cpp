/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "painter.h"
#include "widget.h"

namespace mui
{

    Painter::Painter()
    {}

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
        cairo_save(m_cr.get());
    }

    void Painter::end()
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

    Painter& Painter::draw_rectangle(const Rect& rect)
    {
        cairo_rectangle(m_cr.get(),
                        rect.x,
                        rect.y,
                        rect.w,
                        rect.h);
        cairo_stroke(m_cr.get());

        return *this;
    }

    Painter& Painter::draw_fillrectangle(const Rect& rect)
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

    Painter& Painter::draw_line(const Point& start, const Point& end)
    {
        cairo_move_to(m_cr.get(), start.x, start.y);
        cairo_line_to(m_cr.get(), end.x, end.y);
        return *this;
    }

    Painter& Painter::draw_line(const Point& start, const Point& end, float width)
    {
        set_line_width(width);
        cairo_move_to(m_cr.get(), start.x, start.y);
        cairo_line_to(m_cr.get(), end.x, end.y);
        cairo_stroke(m_cr.get());
        return *this;
    }

    Painter& Painter::draw_image(const Point& point, shared_cairo_surface_t surface, bool bw)
    {
        double w = cairo_image_surface_get_width(surface.get());
        double h = cairo_image_surface_get_height(surface.get());
        cairo_set_source_surface(m_cr.get(), surface.get(), point.x, point.y);
        cairo_rectangle(m_cr.get(), point.x, point.y, w, h);
        cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_OVER);
        cairo_fill(m_cr.get());

        if (bw)
        {
            cairo_set_source_rgb(m_cr.get(), 0, 0, 0);
            cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_HSL_COLOR);
            cairo_mask_surface(m_cr.get(),
                               surface.get(),
                               point.x,
                               point.y);
        }
        return *this;
    }

    /**
     * @param rect The source rect to copy.
     * @param point The destination point.
     */
    Painter& Painter::draw_image(const Rect& rect, const Point& point, shared_cairo_surface_t surface)
    {
#if 1
        cairo_set_source_surface(m_cr.get(), surface.get(), point.x - rect.x, point.y - rect.y);
        cairo_rectangle(m_cr.get(), point.x, point.y, rect.w, rect.h);
        cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_OVER);
        cairo_fill(m_cr.get());
#else
        cairo_set_source_surface(m_cr.get(), surface.get(), point.x, point.y);
        cairo_rectangle(m_cr.get(), rect.x, rect.y, rect.w, rect.h);
        cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_OVER);
        cairo_fill(m_cr.get());
#endif
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
        cairo_select_font_face(m_cr.get(),
                               font.face().c_str(),
                               CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cr.get(), font.size());
        return *this;
    }

    Painter& Painter::draw_text(const Point& point, const std::string& str)
    {
        cairo_text_extents_t textext;
        cairo_text_extents(m_cr.get(), str.c_str(), &textext);

        cairo_move_to(m_cr.get(), point.x + textext.x_bearing,
                      point.y - textext.y_bearing);
        cairo_show_text(m_cr.get(), str.c_str());
        cairo_stroke(m_cr.get());
        return *this;
    }

    Painter& Painter::draw_text(const Rect& rect, const std::string& str, int align, int standoff)
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
            int srx,
            int srcy,
            int width,
            int height,
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

#if 0
    void Painter::draw_border(const Rect& rect)
    {
        //auto cr = screen()->context();

        //cairo_save(m_cr.get());

        double rx = rect.x,
               ry = rect.y,
               width = rect.w,
               height = rect.h,
               aspect = 1.0,
               corner_radius = 50/*height*/ / 10.0;

        double radius = corner_radius / aspect;
        double degrees = M_PI / 180.0;

        cairo_new_sub_path(m_cr.get());
        cairo_arc(m_cr.get(), rx + width - radius, ry + radius, radius, -90 * degrees, 0 * degrees);
        cairo_arc(m_cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90 * degrees);
        cairo_arc(m_cr.get(), rx + radius, ry + height - radius, radius, 90 * degrees, 180 * degrees);
        cairo_arc(m_cr.get(), rx + radius, ry + radius, radius, 180 * degrees, 270 * degrees);
        cairo_close_path(m_cr.get());

        /*
            cairo_set_source_rgba(m_cr.get(),
                                  bg.redf(),
                                  bg.greenf(),
                                  bg.bluef(),
                                  bg.alphaf());
            cairo_fill_preserve(m_cr.get());
        */

        /*
            cairo_set_source_rgba(m_cr.get(),
                                  border.redf(),
                                  border.greenf(),
                                  border.bluef(),
                                  border.alphaf());
        */
        cairo_set_line_width(m_cr.get(), 1.0);
        cairo_stroke(m_cr.get());

        //cairo_restore(m_cr.get());
    }
#endif

#if 0
    void Painter::draw_box(const Rect& rect)
    {
        double rx = rect.x,
               ry = rect.y,
               width = rect.w,
               height = rect.h,
               aspect = 1.0,
               corner_radius = 50/*height*/ / 10.0;

        double radius = corner_radius / aspect;
        double degrees = M_PI / 180.0;

        cairo_new_sub_path(cr.get());
        cairo_arc(cr.get(), rx + width - radius, ry + radius, radius, -90 * degrees, 0 * degrees);
        cairo_arc(cr.get(), rx + width - radius, ry + height - radius, radius, 0 * degrees, 90 * degrees);
        cairo_arc(cr.get(), rx + radius, ry + height - radius, radius, 90 * degrees, 180 * degrees);
        cairo_arc(cr.get(), rx + radius, ry + radius, radius, 180 * degrees, 270 * degrees);
        cairo_close_path(cr.get());

        /*cairo_set_source_rgba(cr.get(),
                              bg.redf(),
                              bg.greenf(),
                              bg.bluef(),
                              bg.alphaf());
        */
        //cairo_fill_preserve(cr.get());
        cairo_fill(cr.get());

        /*
            cairo_set_source_rgba(cr.get(),
                                  border.redf(),
                                  border.greenf(),
                                  border.bluef(),
                                  border.alphaf());
            cairo_set_line_width(cr.get(), 1.0);
            cairo_stroke(cr.get());
        */
    }
#endif


}
