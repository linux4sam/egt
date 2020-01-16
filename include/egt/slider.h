/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SLIDER_H
#define EGT_SLIDER_H

#include <cassert>
#include <egt/detail/flags.h>
#include <egt/detail/math.h>
#include <egt/detail/meta.h>
#include <egt/valuewidget.h>
#include <memory>

namespace egt
{
inline namespace v1
{
class Frame;

/**
 * This is a slider that can be used to select value from a range.
 *
 * @image html widget_slider1.png
 * @image latex widget_slider1.png "widget_slider1" width=5cm
 * @image html widget_slider2.png
 * @image latex widget_slider2.png "widget_slider2" height=5cm
 *
 * @ingroup controls
 */
class EGT_API Slider : public ValueRangeWidget<int>
{
public:

    enum class SliderFlag
    {
        /**
         * Draw a rectangle handle.
         */
        rectangle_handle = detail::bit(0),

        /**
         * Draw a square handle.
         */
        square_handle = detail::bit(1),

        /**
         * Draw a round handle.
         */
        round_handle = detail::bit(2),

        /**
         * Show range labels.
         */
        show_labels = detail::bit(3),

        /**
         * Show value label.
         */
        show_label = detail::bit(4),

        /**
         * Horizontal slider origin (value min()), is to the left. Vertical is at
         * the bottom. Setting this flag will flip this origin.
         */
        origin_opposite = detail::bit(5),

        /**
         * Solid color line.
         */
        consistent_line = detail::bit(6),
    };

    using SliderFlags = detail::Flags<Slider::SliderFlag>;

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal Orientation.
     */
    explicit Slider(const Rect& rect, int min = 0, int max = 100, int value = 0,
                    Orientation orient = Orientation::horizontal) noexcept;

    /**
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal Orientation.
     */
    Slider(int min = 0, int max = 100, int value = 0,
           Orientation orient = Orientation::horizontal) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal Orientation.
     */
    Slider(Frame& parent, const Rect& rect, int min = 0, int max = 100, int value = 0,
           Orientation orient = Orientation::horizontal) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal Orientation.
     */
    explicit Slider(Frame& parent, int min = 0, int max = 100, int value = 0,
                    Orientation orient = Orientation::horizontal) noexcept;

    virtual void handle(Event& event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    using ValueRangeWidget<int>::value;

    virtual int value(int value) override
    {
        int orig = m_value;

        assert(m_max > m_min);

        if (value > m_max)
            value = m_max;

        if (value < m_min)
            value = m_min;

        if (detail::change_if_diff<>(m_value, value))
        {
            damage();

            // live update to handlers?
            if (false)
            {
                on_value_changed.invoke();
            }
            else
                m_invoke_pending = true;
        }

        return orig;
    }

    /**
     * Get the Orientation.
     */
    inline Orientation orient() const { return m_orient; }

    /**
     * Set the Orientation.
     */
    inline void orient(Orientation orient)
    {
        if (detail::change_if_diff<>(m_orient, orient))
            damage();
    }

    inline const SliderFlags& slider_flags() const { return m_slider_flags; }

    inline SliderFlags& slider_flags() { return m_slider_flags; }

    virtual ~Slider() = default;

protected:

    /// Convert a value to an offset.
    inline int to_offset(int value) const
    {
        auto b = content_area();
        if (m_orient == Orientation::horizontal)
            return egt::detail::normalize<float>(value, m_min, m_max, 0, b.width() - handle_width());
        else
            return egt::detail::normalize<float>(value, m_min, m_max, 0, b.height() - handle_height());
    }

    /// Convert an offset to value.
    inline int to_value(int offset) const
    {
        auto b = content_area();
        if (m_orient == Orientation::horizontal)
            return egt::detail::normalize<float>(offset, 0, b.width() - handle_width(), m_min, m_max);
        else
            return egt::detail::normalize<float>(offset, 0, b.height() - handle_height(), m_min, m_max);
    }

    int handle_width() const;
    int handle_height() const;
    Rect handle_box() const;
    Rect handle_box(int value) const;

    virtual void draw_label(Painter& painter, int value);
    virtual void draw_handle(Painter& painter);
    virtual void draw_line(Painter& painter, float xp, float yp);

    Orientation m_orient{Orientation::horizontal};
    bool m_invoke_pending{false};

    /**
     * Slider flags.
     */
    SliderFlags m_slider_flags{};

    int m_start_offset{0};
};

}
}

#endif
