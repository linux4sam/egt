/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/image.h"
#include "egt/painter.h"
#include "egt/widget.h"
#include <cairo.h>

namespace egt
{
    inline namespace v1
    {
        Painter::Painter(shared_cairo_t cr)
            : m_cr(cr)
        {
            //begin();
        }

        Painter::~Painter()
        {
            //end();
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
            fill();

            return *this;
        }

        Painter& Painter::set_line_width(float width)
        {
            cairo_set_line_width(m_cr.get(), width);

            return *this;
        }

        Painter& Painter::point(const Point& p)
        {
            cairo_move_to(m_cr.get(), p.x, p.y);

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

        Painter& Painter::draw_image(const Point& point, const Image& image, bool bw)
        {
            if (image.empty())
                return *this;

            AutoSaveRestore sr(*this);

            if (bw)
                cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_SOFT_LIGHT);
            cairo_set_source_surface(m_cr.get(), image.surface().get(), point.x, point.y);
            cairo_rectangle(m_cr.get(), point.x, point.y, image.size().w, image.size().h);
            fill();

            return *this;
        }

        Painter& Painter::draw_image(const Rect& rect, const Point& point, const Image& image)
        {
            if (image.empty())
                return *this;

            cairo_set_source_surface(m_cr.get(), image.surface().get(),
                                     point.x - rect.x, point.y - rect.y);
            cairo_rectangle(m_cr.get(), point.x, point.y, rect.w, rect.h);
            fill();

            return *this;
        }

        Painter& Painter::draw_image(const Image& image,
                                     const Rect& dest,
                                     alignmask align, int margin, bool bw)
        {
            if (image.empty())
                return *this;

            Rect target = Widget::align_algorithm(image.size(), dest, align, margin);

            if (true)
                draw_image(target.point(), image, bw);
            else
                paint_surface_with_drop_shadow(
                    image.surface().get(),
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

        Painter& Painter::arc(const Arc& arc)
        {
            cairo_arc(m_cr.get(), arc.center.x, arc.center.y,
                      arc.radius, arc.angle1, arc.angle2);

            return *this;
        }

        Painter& Painter::draw_arc(const Arc& arc)
        {
            this->arc(arc);
            stroke();

            return *this;
        }

        Painter& Painter::circle(const Circle& circle)
        {
            arc(circle);

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
            if (!str.empty())
                cairo_text_extents(m_cr.get(), str.c_str(), &textext);
            else
                cairo_text_extents(m_cr.get(), "I", &textext);

            Rect target = Widget::align_algorithm(Size(textext.width,
                                                  textext.height),
                                                  rect,
                                                  align,
                                                  standoff);

            cairo_move_to(m_cr.get(), target.x - textext.x_bearing,
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

        bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                             const Rect& rhs, shared_cairo_surface_t rimage)
        {
            if (Rect::intersect(lhs, rhs))
            {
                unsigned int* ldata = reinterpret_cast<unsigned int*>(cairo_image_surface_get_data(limage.get()));
                unsigned int* rdata = reinterpret_cast<unsigned int*>(cairo_image_surface_get_data(rimage.get()));

                Rect i = Rect::intersection(lhs, rhs);

                for (int y = i.top(); y < i.bottom(); y++)
                {
                    for (int x = i.left(); x < i.right(); x++)
                    {
                        unsigned int l = ldata[(x - lhs.left()) + (y - lhs.top()) * lhs.w];
                        unsigned int r = rdata[(x - rhs.left()) + (y - rhs.top()) * rhs.w];

                        if ((l >> 24 & 0xff) && (r >> 24 & 0xff))
                        {
                            return true;
                        }
                    }
                }
            }

            return false;
        }

        bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                             const Point& rhs)
        {
            if (Rect::intersect(lhs, rhs))
            {
                unsigned int* ldata = reinterpret_cast<unsigned int*>(cairo_image_surface_get_data(limage.get()));

                for (int y = lhs.top(); y < lhs.bottom(); y++)
                {
                    for (int x = lhs.left(); x < lhs.right(); x++)
                    {
                        unsigned int l = ldata[(x - lhs.left()) + (y - lhs.top()) * lhs.w];

                        if (l >> 24 & 0xff)
                        {
                            return true;
                        }
                    }
                }
            }

            return false;
        }

    }
}
