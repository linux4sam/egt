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
#include <egt/detail/flags.h>
#include <egt/detail/meta.h>
#include <egt/font.h>
#include <egt/geometry.h>
#include <egt/types.h>
#include <string>

namespace egt
{
inline namespace v1
{

class Image;

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
class EGT_API Painter : private detail::NonCopyable<Painter>
{
public:

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
    virtual void save();

    /**
     * Restore the previous saved state of the current context.
     *
     * @see AutoSaveRestore
     */
    virtual void restore();

    /**
     * Push a group onto the stack.
     *
     * @see AutoGroup
     */
    virtual void push_group();

    /**
     * Pop a group off the stack and automatically make it the source.
     *
     * @see AutoGroup
     */
    virtual void pop_group();

    /**
     * Set the current color.
     */
    virtual Painter& set(const Color& color);

    /**
     * Set the active font.
     */
    virtual Painter& set(const Font& font);

    /**
     * Set the current line width.
     *
     * @param[in] width Line width.
     */
    virtual Painter& line_width(float width);

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
    virtual Painter& draw(const Image& image);

    /**
     * @param[in] rect The source rect to copy.
     * @param[in] image The image surface to draw.
     */
    virtual Painter& draw(const Rect& rect,
                          const Image& image);

    enum class TextDrawFlag : uint32_t
    {
        shadow = detail::bit(0),
    };

    using TextDrawFlags = detail::Flags<TextDrawFlag>;

    /**
     * Draw text inside the specified rectangle.
     */
    virtual Painter& draw(const std::string& str, const TextDrawFlags& flags = {});

    virtual Painter& clip();

    virtual Painter& fill();

    virtual Painter& paint();

    virtual Painter& paint(float alpha);

    virtual Painter& stroke();

    virtual Size text_size(const std::string& text);

    virtual Size font_size(const std::string& text);

    virtual void color_at(const Point& point, const Color& color) noexcept;
    static void color_at(cairo_surface_t* image, const Point& point, const Color& color) noexcept;
    virtual Color color_at(const Point& point) noexcept;
    static Color color_at(cairo_surface_t* image, const Point& point) noexcept;

    virtual Painter& flood(const Point& point, const Color& color);

    static void flood(cairo_surface_t* image,
                      const Point& point, const Color& color);

    /**
     * Get the current underlying context the painter is using.
     */
    inline shared_cairo_t context() const
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

    virtual ~Painter() = default;

protected:

    /**
     * Cairo context.
     */
    shared_cairo_t m_cr;
};

}
}

#endif
