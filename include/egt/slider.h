/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SLIDERWIDGET_H
#define EGT_SLIDERWIDGET_H

#include <egt/valuewidget.h>
#include <egt/detail/math.h>

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

    Slider(orientation orient) noexcept;

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

        auto handle = handle_box();

        if (m_orientation == orientation::HORIZONTAL)
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

    virtual void set_value(int v) override
    {
        if (v > m_max)
            v = m_max;

        if (v < m_min)
            v = m_min;

        if (v != m_value)
        {
            m_value = v;
            damage();

            // live update to handlers?
            if (false)
                this->invoke_handlers(eventid::PROPERTY_CHANGED);
            else
                m_invoke_pending = true;
        }
    }

    virtual ~Slider();

protected:

    // position to offset
    inline int normalize(int pos) const
    {
        if (m_orientation == orientation::HORIZONTAL)
            return egt::detail::normalize<float>(pos, m_min, m_max, 0, w() - handle_dim());
        else
            return egt::detail::normalize<float>(pos, m_min, m_max, 0, h() - handle_dim());
    }

    // offset to position
    inline int denormalize(int offset) const
    {
        if (m_orientation == orientation::HORIZONTAL)
            return egt::detail::normalize<float>(offset, 0, w() - handle_dim(), m_min, m_max);
        else
            return egt::detail::normalize<float>(offset, 0, h() - handle_dim(), m_min, m_max);
    }

    Rect handle_box() const;
    int handle_dim() const;

    int m_moving_offset{0};
    int m_start_pos{0};
    orientation m_orientation;
    bool m_invoke_pending{false};
};

}
}

#endif
