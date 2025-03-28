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

#include <egt/color.h>
#include <egt/detail/meta.h>
#include <egt/flags.h>
#include <egt/font.h>
#include <egt/geometry.h>
#include <egt/pattern.h>
#include <egt/types.h>
#include <functional>
#include <iosfwd>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

namespace detail
{
class InternalContext;
}

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
     * Supported types of anti-aliasing.
     */
    enum class AntiAlias
    {
        system, // default
        none,
        gray,
        subpixel,
        fast,
        good,
        best,
    };

    /**
     * Supported line caps.
     */
    enum class LineCap
    {
        butt,
        round,
        square,
    };

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

    explicit Painter(Surface& surface) noexcept;
    ~Painter();

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
     * Configure low fidelity options.
     *
     * This configures settings related to font hinting, font aliasing, and
     * shape aliasing.
     */
    void low_fidelity();

    /**
     * Configure high fidelity options.
     *
     * This configures settings related to font hinting, font aliasing, and
     * shape aliasing.
     */
    void high_fidelity();

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
     * Set the current line cap.
     *
     * @param[in] value Line cap.
     */
    Painter& line_cap(Painter::LineCap value);

    /**
     * Get the current line cap.
     */
    EGT_NODISCARD Painter::LineCap line_cap() const;

    /**
     * Set the dash pattern to be used by stroke().
     *
     * @param[in] dashes An array specifying alternate lengths of on and off stroke portions.
     * @param[in] num_dashes The length of the dashes array.
     * @param[in] offset An offset into the dash pattern at which the stroke should start.
     */
    Painter& set_dash(const double* dashes, size_t num_dashes, double offset);

    /**
     * Set the type of anti-aliasing.
     *
     * @param[in] value Anti-aliasing type.
     */
    Painter& antialias(Painter::AntiAlias value);

    /**
     * Get the current type of anti-aliasing.
     */
    EGT_NODISCARD Painter::AntiAlias antialias() const;

    /**
     * Set the alpha blending state either enabled or disabled.
     *
     * @param[in] enabled Whether the alpha blending is to be enabled.
     *
     * @note disabling the alpha blending is like forcing the destination alpha
     * to zero.
     */
    Painter& alpha_blending(bool enabled);

    /**
     * Get the alpha blending state, either enabled or disabled.
     */
    EGT_NODISCARD bool alpha_blending() const;

    /**
     * Move to a point.
     *
     * @param[in] point The point.
     */
    template<class T>
    Painter& draw(const PointType<T, detail::Compatible::normal>& point)
    {
        return move_to(point);
    }

    template<class T>
    Painter& line(const PointType<T, detail::Compatible::normal>& point)
    {
        return line_to(point);
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
        return move_to(start).line_to(end);
    }

    /**
     * Create a line.
     *
     * @param[in] line The line.
     */
    template<class T>
    Painter& draw(const LineType<T>& line)
    {
        return move_to(line.start()).line_to(line.end());
    }

    /**
     * Create a rectangle.
     *
     * @param[in] rect The rectangle.
     */
    template<class T>
    Painter& draw(const RectType<T>& rect)
    {
        return rectangle(rect);
    }

    /**
     * Create an arc.
     *
     * @param[in] arc The arc.
     */
    template<class T>
    Painter& draw(const ArcType<T>& arc)
    {
        return this->arc(arc);
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
    Painter& mask(const Surface& surface, const PointF& point = {});

    Painter& mask(const Image& image, const Point& point = {});

    /**
     * @param[in] surface The surface source to draw.
     * @param[in] point The position of the surface origin.
     * @param[in] rect The rectangle to draw, if any, the whole surface otherwise.
     */
    Painter& draw(const Surface& surface, const PointF& point, const RectF& rect = {});

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

    Painter& fill_preserve();

    Painter& paint();

    Painter& paint(float alpha);

    Painter& stroke();

    Painter& move_to(const PointF& point);

    Painter& line_to(const PointF& point);

    Painter& rectangle(const RectF& rect);

    Painter& arc(const ArcF& arc);

    Painter& translate(const PointF& point);

    Painter& translate(const Point& point);

    Painter& scale(float sx, float sy);

    Painter& rotate(float angle);

    Painter& show_text(const char* utf8);

    Painter& show_text(const std::string& str) { return show_text(str.c_str()); }

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
    Color color_at(const Point& point) noexcept;

    Painter& flood(const Point& point, const Color& color);

    /**
     * Get the current underlying context the painter is using.
     */
    EGT_NODISCARD const detail::InternalContext& context() const { return *m_cr; }

    /**
     * Get the target surface the painter is using.
     */
    EGT_NODISCARD const Surface& target() const { return m_surface; }

    EGT_NODISCARD Surface& target() { return m_surface; }

    /**
     * Claim the painter for being used by the CPU.
     *
     * @param[in] skip_source The boolean telling whether the source surface
     *                        should be ignored.
     *
     * Wait for all GPU operations, if any, to complete. Synchronize the target
     * surface of the Painter instance. Also synchronize the source surface, if
     * any, unless @skip_source is 'true'. Indeed, the source should be skipped
     * if the rendering operations to come won't use it because a new source
     * is about to be set.
     */
    void sync_for_cpu(bool skip_source = false) const;

protected:

    /**
     * Internal state.
     */
    SubordinateFilter m_subordinate_filter;

    /**
     * Internal context.
     */
    std::unique_ptr<detail::InternalContext> m_cr;

    Surface& m_surface;
};

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const Painter::LineCap& cap);

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const Painter::AntiAlias& antialias);

}
}

#endif
