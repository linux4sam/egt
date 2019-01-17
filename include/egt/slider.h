/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SLIDERWIDGET_H
#define EGT_SLIDERWIDGET_H

#include <cassert>
#include <egt/bitmask.h>
#include <egt/detail/math.h>
#include <egt/detail/mousegesture.h>
#include <egt/valuewidget.h>

namespace egt
{
inline namespace v1
{

/**
 * This is a slider that can be used to select value from a range.
 *
 * @image html widget_slider1.png
 * @image latex widget_slider1.png "widget_slider1" width=5cm
 * @image html widget_slider2.png
 * @image latex widget_slider2.png "widget_slider2" height=5cm
 */
class Slider : public ValueRangeWidget<int>
{
public:

    enum class flags
    {
        RECTANGLE_HANDLE = 1 << 0,
        SQUARE_HANDLE = 1 << 1,
        ROUND_HANDLE = 1 << 2,
        SHOW_LABELS = 1 << 3,
        SHOW_LABEL = 1 << 4,

        /**
         * Horizontal slider origin (value min()), is to the left. Vertical is at
         * the bottom. Setting this flag will flip this origin.
         */
        ORIGIN_OPPOSITE = 1 << 5,

        /**
         *
         */
        CONSISTENT_LINE = 1 << 6,
    };

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] min Minimum value for the range.
     * @param[in] max Maximum value in the range.
     * @param[in] value Current value in the range.
     * @param[in] orient Vertical or horizontal orientation.
     */
    Slider(const Rect& rect = Rect(), int min = 0, int max = 100, int value = 0,
           orientation orient = orientation::HORIZONTAL) noexcept;

    Slider(int min, int max, int value,
           orientation orient = orientation::HORIZONTAL) noexcept;

    explicit Slider(orientation orient) noexcept;

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

#define OPTIMIZE_SLIDER_DRAW
#ifdef OPTIMIZE_SLIDER_DRAW
    /**
     * By default, damage() will damage the entire box.  However, this
     * widget can display on a rectangle significantly less that its box().
     */
    virtual void damage() override
    {
        if (box().empty())
            return;

        // can't optimize without more work below to include label - SHOW_LABEL
        // moves as well
        if (is_set(flags::SHOW_LABEL))
        {
            Widget::damage();
            return;
        }

        auto handle = handle_box();

        if (m_orient == orientation::HORIZONTAL)
        {
            damage(Rect(x() - 1,
                        handle.y - 1,
                        w() + 2,
                        handle.h + 2));
        }
        else
        {
            damage(Rect(handle.x  - 1,
                        y() - 1,
                        handle.w + 2,
                        h() + 2));
        }
    }

    virtual void damage(const Rect& rect) override
    {
        Widget::damage(rect);
    }
#endif

    virtual void set_value(int value) override
    {
        assert(m_max > m_min);

        if (value > m_max)
            value = m_max;

        if (value < m_min)
            value = m_min;

        if (value != m_value)
        {
            m_value = value;
            damage();

            // live update to handlers?
            if (false)
                this->invoke_handlers(eventid::PROPERTY_CHANGED);
            else
                m_invoke_pending = true;
        }
    }

    inline void slider_flags(flags flags)
    {
        if (flags != m_slider_flags)
        {
            m_slider_flags = flags;
            damage();
        }
    }

    virtual ~Slider();

protected:

    /// Convert a value to an offset.
    inline int to_offset(int value) const
    {
        if (m_orient == orientation::HORIZONTAL)
            return egt::detail::normalize<float>(value, m_min, m_max, 0, w() - handle_width());
        else
            return egt::detail::normalize<float>(value, m_min, m_max, 0, h() - handle_height());
    }

    /// Convert an offset to value.
    inline int to_value(int offset) const
    {
        if (m_orient == orientation::HORIZONTAL)
            return egt::detail::normalize<float>(offset, 0, w() - handle_width(), m_min, m_max);
        else
            return egt::detail::normalize<float>(offset, 0, h() - handle_height(), m_min, m_max);
    }

    int handle_width() const;
    int handle_height() const;
    Rect handle_box() const;
    Rect handle_box(int value) const;

    virtual void draw_label(Painter& painter, int value);
    virtual void draw_handle(Painter& painter);
    virtual void draw_line(Painter& painter, float xp, float yp);

    orientation m_orient;
    bool m_invoke_pending{false};

    flags m_slider_flags{flags::RECTANGLE_HANDLE};

    using Swipe = detail::MouseGesture<int>;

    Swipe m_mouse;

    inline bool is_set(flags flag) const;
};

ENABLE_BITMASK_OPERATORS(Slider::flags)

bool Slider::is_set(flags flag) const
{
    return ((m_slider_flags & flag) == flag);
}

}
}

#endif
