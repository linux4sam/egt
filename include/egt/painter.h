/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PAINTER_H
#define EGT_PAINTER_H

/**
 * @file
 * @brief Painter interface.
 */

#include <cairo.h>
#include <egt/color.h>
#include <egt/detail/meta.h>
#include <egt/flags.h>
#include <egt/font.h>
#include <egt/geometry.h>
#include <egt/pattern.h>
#include <egt/types.h>
#include <functional>
#include <string>

namespace egt
{
inline namespace v1
{

class Image;
class Surface;
class Widget;

/**
 * @defgroup drawing Drawing Classes
 * Drawing related functionality.
 */

/**
 * Drawing interface for 2D graphics.
 *
 * This is the interface for 2D drawing primitives that makes working with and
 * drawing EGT primitives easier.
 *
 * @ingroup drawing
 */
class EGT_API Painter
{
public:

    /**
     * Return true if the subordinate widget is filtered out, hence should
     * not be drawn by the painter.
     */
    using SubordinateFilter = std::function<bool(const Widget&)>;

    /**
     * Scoped save() and restore() for a Painter.
     *
     * You are encouraged to use this instead of manually calling
     * Painter::save() and Painter::restore().
     *
     * @b Example
     * @code{.cpp}
     * void SomeWidget::paint(Painter& painter)
     * {
     *     Painter::AutoSaveRestore sr(painter);
     *
     *     // painter state will be restored when sr is destructed
     * }
     * @endcode
     */
    struct AutoSaveRestore
    {
        explicit AutoSaveRestore(Painter& painter)
            : m_painter(painter)
        {
            m_painter.save();
        }

        AutoSaveRestore(const AutoSaveRestore&) = delete;
        AutoSaveRestore& operator=(const AutoSaveRestore&) = delete;
        AutoSaveRestore(AutoSaveRestore&&) = delete;
        AutoSaveRestore& operator=(AutoSaveRestore&&) = delete;

        ~AutoSaveRestore()
        {
            m_painter.restore();
        }

        Painter& m_painter;
    };

    /**
     * You are encouraged to use this instead of manually calling
     * Painter::push_group() and Painter::pop_group().
     *
     * @b Example
     * @code{.cpp}
     * {
     *     Painter::AutoGroup group(painter);
     *
     *     // group automatically popped when out of scope
     * }
     * @endcode
     */
    struct AutoGroup
    {
        explicit AutoGroup(Painter& painter)
            : m_painter(painter)
        {
            m_painter.push_group();
        }

        AutoGroup(const AutoGroup&) = delete;
        AutoGroup& operator=(const AutoGroup&) = delete;
        AutoGroup(AutoGroup&&) = delete;
        AutoGroup& operator=(AutoGroup&&) = delete;

        ~AutoGroup()
        {
            m_painter.pop_group();
        }

        Painter& m_painter;
    };

    Painter() = delete;

    /**
     * Construct a Painter with an existing context.
     *
     * @todo Painter needs to come from the Screen. This constructor should
     * be hidden and you should have to get a custom version of it from the
     * Screen for drawing. A default software/GPU painter can be created,
     * otherwise for something like X11 we should be using X11 to paint.
     */
    explicit Painter(shared_cairo_t cr) noexcept;

    /**
     * Save the state of the current context.
     *
     * @see AutoSaveRestore
     */
    void save();

    /**
     * Restore the previous saved state of the current context.
     *
     * @see AutoSaveRestore
     */
    void restore();

    /**
     * Apply the current subordinate filter.
     *
     * @param[in] subordinate The widget to be tested.
     *
     * @return true if the subordinate widget is filtered out, hence should
     * not be drawn by the painter.
     */
    EGT_NODISCARD bool filter_subordinate(const Widget& subordinate) const;

    /**
     * Set the subordinate filter (nullptr to remove the current filter).
     *
     * @param[in] subordinate_filter The new subordinate filter.
     *
     * @return a copy of the previous subordinate filter.
     */
    SubordinateFilter set_subordinate_filter(const SubordinateFilter& subordinate_filter);

    /**
     * Restore the subordinate filter from a previous value.
     *
     * @param[in] subordinate_filter The saved subordinate filter to restore.
     */
    void restore_subordinate_filter(SubordinateFilter&& subordinate_filter);

    /**
     * Push a group onto the stack.
     *
     * @see AutoGroup
     */
    void push_group();

    /**
     * Pop a group off the stack and automatically make it the source.
     *
     * @see AutoGroup
     */
    void pop_group();

    /**
     * Set the current color.
     */
    Painter& set(const Pattern& pattern);

    /**
     * Set the active font.
     */
    Painter& set(const Font& font);

    /**
     * Set the current line width.
     *
     * @param[in] width Line width.
     */
    Painter& line_width(float width);

    /**
     * Move to a point.
     *
     * @param[in] point The point.
     */
    template<class T>
    Painter& draw(const PointType<T, detail::Compatible::normal>& point)
    {
        cairo_move_to(m_cr.get(), point.x(), point.y());

        return *this;
    }

    template<class T>
    Painter& line(const PointType<T, detail::Compatible::normal>& point)
    {
        cairo_line_to(m_cr.get(), point.x(), point.y());

        return *this;
    }

    /**
     * Create a line from the start point to the end point.
     *
     * @param[in] start The point.
     * @param[in] end The point.
     */
    template<class T>
    Painter& draw(const T& start, const T& end)
    {
        cairo_move_to(m_cr.get(), start.x(), start.y());
        cairo_line_to(m_cr.get(), end.x(), end.y());

        return *this;
    }

    /**
     * Create a line.
     *
     * @param[in] line The line.
     */
    template<class T>
    Painter& draw(const LineType<T>& line)
    {
        cairo_move_to(m_cr.get(), line.start().x(), line.start().y());
        cairo_line_to(m_cr.get(), line.end().x(), line.end().y());

        return *this;
    }

    /**
     * Create a rectangle.
     *
     * @param[in] rect The rectangle.
     */
    template<class T>
    Painter& draw(const RectType<T>& rect)
    {
        if (rect.empty())
            return *this;

        cairo_rectangle(m_cr.get(),
                        rect.x(),
                        rect.y(),
                        rect.width(),
                        rect.height());

        return *this;
    }

    /**
     * Create an arc.
     *
     * @param[in] arc The arc.
     */
    template<class T>
    Painter& draw(const ArcType<T>& arc)
    {
        if (arc.empty())
            return *this;

        cairo_arc(m_cr.get(), arc.center().x(), arc.center().y(),
                  arc.radius(), arc.angle1(), arc.angle2());

        return *this;
    }

    /**
     * Create a circle.
     *
     * @param[in] arc The circle.
     */
    template<class T>
    Painter& draw(const CircleType<T>& arc)
    {
        if (arc.empty())
            return *this;

        cairo_arc(m_cr.get(), arc.center().x(), arc.center().y(),
                  arc.radius(), arc.angle1(), arc.angle2());

        return *this;
    }

    /**
     * Draw an image surface at the specified point.
     */
    Painter& draw(const Image& image);

    /**
     * Set the source pattern from a Pattern.
     */
    Painter& source(const Pattern& pattern);

    /**
     * Set the source pattern from a Color.
     */
    Painter& source(const Color& color);

    /**
     * Set the source pattern from a cairo_surface_t.
     */
    Painter& source(cairo_surface_t* surface, const PointF& point = {});

    /**
     * Set the source pattern from a shared_cairo_surface_t.
     */
    Painter& source(const shared_cairo_surface_t& surface, const PointF& point = {});

    /**
     * Set the source pattern from a Surface.
     */
    Painter& source(const Surface& surface, const PointF& point = {});

    /**
     * Set the source pattern from an Image.
     */
    Painter& source(const Image& image, const PointF& point = {});

    /**
     * Draw an image as a mask.
     */
    Painter& mask(const Image& image, const Point& point = {});

    /**
     * @param[in] surface The surface source to draw.
     * @param[in] point The position of the surface origin.
     * @param[in] rect The rectangle to draw, if any, the whole surface otherwise.
     */
    Painter& draw(cairo_surface_t* surface, const PointF& point, const RectF& rect = {});

    /**
     * @param[in] surface The surface source to draw.
     * @param[in] point The position of the surface origin.
     * @param[in] rect The rectangle to draw, if any, the whole surface otherwise.
     */
    Painter& draw(const shared_cairo_surface_t& surface, const PointF& point, const RectF& rect = {});

    /**
     * @param[in] image The image source to draw.
     * @param[in] point The position of the surface origin.
     * @param[in] rect The rectangle to draw, if any, the whole image otherwise.
     */
    Painter& draw(const Image& image, const PointF& point, const RectF& rect = {});

    enum class TextDrawFlag : uint32_t
    {
        shadow = detail::bit(0),
    };

    using TextDrawFlags = Flags<TextDrawFlag>;

    /**
     * Draw text inside the specified rectangle.
     */
    Painter& draw(const std::string& str, const TextDrawFlags& flags = {});

    Painter& clip();

    Painter& fill();

    Painter& paint();

    Painter& paint(float alpha);

    Painter& stroke();

    Painter& translate(const PointF& point);

    Painter& translate(const Point& point);

    Painter& rotate(float angle);

    /**
     * Get the font extents based on the current context, hence taking into
     * account transformations such as rotation or symmetry.
     */
    EGT_NODISCARD Font::FontExtents extents() const;

    /**
     * Get the text extents based on the current context, hence taking into
     * account transformations such as rotation or symmetry.
     *
     * @param[in] text The UTF8 encoded text.
     */
    EGT_NODISCARD Font::TextExtents extents(const std::string& text) const;

    Size text_size(const std::string& text);

    void color_at(const Point& point, const Color& color) noexcept;
    static void color_at(cairo_surface_t* image, const Point& point, const Color& color) noexcept;
    Color color_at(const Point& point) noexcept;
    static Color color_at(cairo_surface_t* image, const Point& point) noexcept;

    Painter& flood(const Point& point, const Color& color);

    static void flood(cairo_surface_t* image,
                      const Point& point, const Color& color);

    /**
     * Get the current underlying context the painter is using.
     */
    EGT_NODISCARD inline shared_cairo_t context() const
    {
        return m_cr;
    }

    /**
     * Get a Size from a surface.
     */
    static inline Size surface_to_size(const shared_cairo_surface_t& surface)
    {
        return {cairo_image_surface_get_width(surface.get()),
                cairo_image_surface_get_height(surface.get())};
    }

    /**
     * Get a Size from a surface.
     */
    static inline Size surface_to_size(cairo_surface_t* surface)
    {
        return {cairo_image_surface_get_width(surface),
                cairo_image_surface_get_height(surface)};
    }

protected:

    /**
     * Internal state.
     */
    SubordinateFilter m_subordinate_filter;

    /**
     * Cairo context.
     */
    shared_cairo_t m_cr;
};

}
}

#endif
