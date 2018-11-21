/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PAINTER_H
#define EGT_PAINTER_H

#include <cairo.h>
#include <memory>
#include <egt/font.h>
#include <egt/geometry.h>
#include <egt/palette.h>
#include <egt/widget.h>
#include <egt/utils.h>

namespace egt
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

        /**
         * @todo Painter needs to come from the Screen. This constructor should
         * be hidden and you should have to get a custom version of it from the
         * Screen for drawing. A default software/GPU painter can be created,
         * otherwise for something like X11 we should be using X11 to paint.
         *
         * Or maybe we leave this as it is and put the drawing abstraction under
         * cairo...
         */
        explicit Painter(shared_cairo_t cr);

        virtual ~Painter();

        /**
        * Save the state of the current context.
         *
               * @see AutoSaveRestore
               */
        virtual void save();

        /**
        * Restore the previous saved state of the current context.
         *
               * @see AutoSaveRestore
               */
        virtual void restore();

        virtual void begin();

        virtual void end();

        /**
         * Set the current color.
         */
        virtual Painter& set_color(const Color& color);

        /**
         * Create a rectangle.
         *
         * @param[in] rect The rectangle.
         */
        virtual Painter& rectangle(const Rect& rect);


        virtual Painter& draw_fill(const Rect& rect);

        /**
         * Set the current line width.
         *
         * @param[in] width Line width.
         */
        virtual Painter& set_line_width(float width);

        /**
         * Create a line from the current point to the specified point.
         *
         * @param[in] end End point.
         */
        virtual Painter& line(const Point& end);

        /**
         * Create a line from the specified start to end point.
         *
         * @param[in] start Start point.
         * @param[in] end End point.
         */
        virtual Painter& line(const Point& start, const Point& end);

        /**
         * Draw an image surface at the specified point.
         */
        virtual Painter& draw_image(const Point& point,
                                    shared_cairo_surface_t surface, bool bw = false);

        virtual Painter& draw_image(const Point& point,
                                    cairo_surface_t* surface, bool bw = false);

        /**
         * @param[in] rect The source rect to copy.
         * @param[in] point The destination point.
         * @param[in] surface The image surface to draw.
         */
        virtual Painter& draw_image(const Rect& rect, const Point& point,
                                    shared_cairo_surface_t surface);

        virtual Painter& arc(const Point& point, float radius, float angle1,
                             float angle2);

        /**
         * Draw an arc.
         */
        virtual Painter& draw_arc(const Point& point, float radius,
                                  float angle1, float angle2);

        virtual Painter& clip();

        virtual void stroke()
        {
            cairo_stroke(m_cr.get());
        }

        virtual void paint()
        {
            cairo_paint(m_cr.get());
        }

        virtual void fill()
        {
            cairo_fill(m_cr.get());
        }

        /**
         * Set the active font.
         */
        virtual Painter& set_font(const Font& font);

        /**
         * Draw text aligned inside the specified rectangle.
         */
        virtual Rect draw_text(const Rect& rect, const std::string& str, alignmask align, int standoff = 5);

        /**
         * @todo Special functions that do a lot, mostly implementing widget
         * theme. Seems as if this maybe needs to be broken out into a theme
         * class.
         *@{
         */
        virtual Rect draw_text(const std::string& text,
                               const Rect& rect,
                               const Color& color = Color::BLACK,
                               alignmask align = alignmask::CENTER,
                               int margin = 5,
                               const Font& font = Font());

        virtual Size text_size(const std::string& text);

        virtual Painter& draw_image(shared_cairo_surface_t image,
                                    const Rect& dest,
                                    alignmask align = alignmask::CENTER,
                                    int margin = 0,
                                    bool bw = false);

        enum class boxtype
        {
            none,
            fill,
            border,
            rounded_border,
            borderfill,
            rounded_borderfill,
            rounded_gradient,
        };

        virtual Painter& draw_box(Widget& widget,
                                  boxtype type = boxtype::borderfill,
                                  const Rect& rect = Rect());
        /**
         *@}
         */

        shared_cairo_t context() const
        {
            return m_cr;
        }

        void draw_fill_box(const Rect& rect,
                           const Color& bg);

        void draw_border_box(const Rect& rect,
                             const Color& border);

        void draw_border_fill_box(const Rect& rect,
                                  const Color& border,
                                  const Color& bg);

        void draw_rounded_border_box(const Rect& rect,
                                     const Color& border);

        void draw_rounded_borderfill_box(const Rect& rect,
                                         const Color& border,
                                         const Color& bg);

        void draw_rounded_gradient_box(const Rect& rect,
                                       const Color& border,
                                       const Color& bg);
    protected:

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


        shared_cairo_t m_cr;

    private:

        Painter() = delete;
    };

    bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                         const Rect& rhs, shared_cairo_surface_t rimage);

    bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                         const Point& rhs);

}

#endif
