/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SLIDERWIDGET_H
#define EGT_SLIDERWIDGET_H

#include <cassert>
#include <egt/detail/math.h>
#include <egt/flags.h>
#include <egt/valuewidget.h>
#include <memory>

namespace egt
{
inline namespace v1
{
class Frame;

/**
 * @brief This is a slider that can be used to select value from a range.
 *
 * @image html widget_slider1.png
 * @image latex widget_slider1.png "widget_slider1" width=5cm
 * @image html widget_slider2.png
 * @image latex widget_slider2.png "widget_slider2" height=5cm
 *
 * @ingroup controls
 */
class Slider : public ValueRangeWidget<int>
{
public:

    enum class flag
    {
        /**
         * Draw a rectangle handle.
         */
        rectangle_handle,

        /**
         * Draw a square handle.
         */
        square_handle,

        /**
         * Draw a round handle.
         */
        round_handle,

        /**
         * Show range labels.
         */
        show_labels,

        /**
         * Show value label.
         */
        show_label,

        /**
         * Horizontal slider origin (value min()), is to the left. Vertical is at
         * the bottom. Setting this flag will flip this origin.
         */
        origin_opposite,

        /**
         * Solid color line.
         */
        consistent_line,
    };

    using flags_type = Flags<Slider::flag>;

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal orientation.
     */
    explicit Slider(const Rect& rect, int min = 0, int max = 100, int value = 0,
                    orientation orient = orientation::horizontal) noexcept;

    /**
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal orientation.
     */
    Slider(int min = 0, int max = 100, int value = 0,
           orientation orient = orientation::horizontal) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal orientation.
     */
    Slider(Frame& parent, const Rect& rect, int min = 0, int max = 100, int value = 0,
           orientation orient = orientation::horizontal) noexcept;

    /**
     * @param[in] parent The parent Frame.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal orientation.
     */
    explicit Slider(Frame& parent, int min = 0, int max = 100, int value = 0,
                    orientation orient = orientation::horizontal) noexcept;

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<Slider>(*this).release());
    }

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    //#define OPTIMIZE_SLIDER_DRAW
#ifdef OPTIMIZE_SLIDER_DRAW
    /**
     * By default, damage() will damage the entire box().  However, this
     * widget can damage a rectangle significantly less than its box().
     */
    virtual void damage() override
    {
        auto b = content_area();

        if (b.empty())
            return;

        // can't optimize without more work below to include label - SHOW_LABEL
        // moves as well
        if (slider_flags().is_set(flag::show_label))
        {
            Widget::damage();
            return;
        }

        auto handlerect = handle_box();

        if (m_orient == orientation::horizontal)
        {
            damage(Rect(b.x - 1,
                        handlerect.y - 1,
                        b.w + 2,
                        handlerect.h + 2));
        }
        else
        {
            damage(Rect(handlerect.x  - 1,
                        b.y - 1,
                        handlerect.w + 2,
                        b.h + 2));
        }
    }

    virtual void damage(const Rect& rect) override
    {
        Widget::damage(rect);
    }
#endif

    virtual int set_value(int value) override
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
                this->invoke_handlers(eventid::property_changed);
            else
                m_invoke_pending = true;
        }

        return orig;
    }

    inline const flags_type& slider_flags() const { return m_slider_flags; }

    inline flags_type& slider_flags() { return m_slider_flags; }

    virtual ~Slider() = default;

protected:

    /// Convert a value to an offset.
    inline int to_offset(int value) const
    {
        auto b = content_area();
        if (m_orient == orientation::horizontal)
            return egt::detail::normalize<float>(value, m_min, m_max, 0, b.w - handle_width());
        else
            return egt::detail::normalize<float>(value, m_min, m_max, 0, b.h - handle_height());
    }

    /// Convert an offset to value.
    inline int to_value(int offset) const
    {
        auto b = content_area();
        if (m_orient == orientation::horizontal)
            return egt::detail::normalize<float>(offset, 0, b.w - handle_width(), m_min, m_max);
        else
            return egt::detail::normalize<float>(offset, 0, b.h - handle_height(), m_min, m_max);
    }

    int handle_width() const;
    int handle_height() const;
    Rect handle_box() const;
    Rect handle_box(int value) const;

    virtual void draw_label(Painter& painter, int value);
    virtual void draw_handle(Painter& painter);
    virtual void draw_line(Painter& painter, float xp, float yp);

    orientation m_orient{orientation::horizontal};
    bool m_invoke_pending{false};

    /**
     * Slider flags.
     */
    flags_type m_slider_flags;

    int m_start_offset{0};
};

}
}

#endif
