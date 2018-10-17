/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_PAINTER_H
#define MUI_PAINTER_H

#include <cairo.h>
#include <memory>
#include <mui/font.h>
#include <mui/geometry.h>
#include <mui/palette.h>
#include <mui/widget.h>
#include <mui/utils.h>

namespace mui
{
    using shared_cairo_surface_t =
        std::shared_ptr<cairo_surface_t>;

    using shared_cairo_t =
        std::shared_ptr<cairo_t>;

    using shared_cairo_pattern_t =
        std::shared_ptr<cairo_pattern_t>;

    /**
     * Drawing interface for 2D graphics.
     *
     * This is the interface for all 2D drawing primitives.
     */
    class Painter : public detail::noncopyable
    {
    public:

        /**
         * Scoped save() and restore() for a Painter.
         *
         * You are encouraged to use this instead of manually calling save() and
         * restore().
         */
        struct AutoSaveRestore
        {
            explicit AutoSaveRestore(Painter& painter)
                : m_painter(painter)
            {
                m_painter.save();
            }

            ~AutoSaveRestore()
            {
                m_painter.restore();
            }

            Painter& m_painter;
        };

        Painter();

        explicit Painter(shared_cairo_t cr);

        virtual ~Painter();

        /**
         * @see AutoSaveRestore
         */
        void save();

        /**
         * @see AutoSaveRestore
         */
        void restore();

        void begin();

        void end();

        Painter& set_color(const Color& color);

        Painter& rectangle(const Rect& rect);

        Painter& draw_fill(const Rect& rect);

        Painter& set_line_width(float width);

        Painter& line(const Point& end);

        Painter& line(const Point& start, const Point& end);

        Painter& draw_image(const Point& point, shared_cairo_surface_t surface, bool bw = false);

        Painter& draw_image(const Rect& rect, const Point& point, shared_cairo_surface_t surface);

        Painter& draw_arc(const Point& point, float radius, float angle1, float angle2);

        Painter& clip();

        void stroke()
        {
            cairo_stroke(m_cr.get());
        }

#if 0
        void paint()
        {
            cairo_paint(m_cr.get());
        }

        void fill()
        {
            cairo_fill(m_cr.get());
        }
#endif

        /**
         * Set the active font.
         */
        Painter& set_font(const Font& font);

        /**
         * Draw text aligned inside the specified rectangle.
         */
        Painter& draw_text(const Rect& rect, const std::string& str, alignmask align, int standoff = 5);

        /**
         * @todo Special functions that do a lot, mostly implementing widget
         * theme. Seems as if this maybe needs to be broken out into a theme
         * class.
         *@{
         */
        Painter& draw_text(const std::string& text,
                           const Rect& rect,
                           const Color& color = Color::BLACK,
                           alignmask align = alignmask::CENTER,
                           int margin = 5,
                           const Font& font = Font());

        Painter& draw_image(shared_cairo_surface_t image,
                            const Rect& dest,
                            alignmask align = alignmask::CENTER,
                            int margin = 0,
                            bool bw = false);

        Painter& draw_basic_box(const Rect& rect,
                                const Color& border,
                                const Color& bg);

        Painter& draw_gradient_box(const Rect& rect,
                                   const Color& border,
                                   bool active = false,
                                   const Color& color = Color::WHITE);
        /**
         *@}
         */

        shared_cairo_t context() const
        {
            return m_cr;
        }

        void paint_surface_with_drop_shadow(cairo_surface_t* source_surface,
                                            int shadow_offset,
                                            double shadow_alpha,
                                            double tint_alpha,
                                            /*int srx,
                                            int srcy,
                                            int width,
                                            int height,*/
                                            int dstx,
                                            int dsty);


    protected:

        shared_cairo_t m_cr;
    };

}

#endif
