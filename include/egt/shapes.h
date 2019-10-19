/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SHAPES_H
#define EGT_SHAPES_H

/**
 * @file
 * @brief Working with shape widgets.
 */

#include <egt/widget.h>
#include <egt/frame.h>
#include <memory>

namespace egt
{
inline namespace v1
{

/**
 * @defgroup shapes Shapes
 * Shape related widgets.
 */

/**
 * Basic Circle Widget.
 *
 * @ingroup shapes
 */
class CircleWidget : public Widget
{
public:

    explicit CircleWidget(const Circle& circle = Circle());

    explicit CircleWidget(Frame& parent, const Circle& circle = Circle());

    inline Circle::dim_type radius() const
    {
        return m_radius;
    }

    virtual void draw(Painter& painter, const Rect&) override;

protected:
    Circle::dim_type m_radius{};
};

/**
 * Basic Line Widget.
 *
 * @ingroup shapes
 */
class LineWidget : public Widget
{
public:

    explicit LineWidget(const Rect& rect = {})
        : Widget(rect)
    {
        set_name("LineWidget" + std::to_string(m_widgetid));
        set_boxtype(Theme::boxtype::none);
    }

    explicit LineWidget(Frame& parent, const Rect& rect = {})
        : LineWidget(rect)
    {
        parent.add(*this);
    }

    virtual void draw(Painter& painter, const Rect&) override;

    inline bool horizontal() const { return m_horizontal; }

    inline void set_horizontal(bool horizontal)
    {
        if (detail::change_if_diff<>(m_horizontal, horizontal))
            damage();
    }

protected:
    bool m_horizontal{true};
};

/**
 * Basic Rectangle Widget.
 *
 * @ingroup shapes
 */
class RectangleWidget : public Widget
{
public:

    explicit RectangleWidget(const Rect& rect = {})
        : Widget(rect)
    {
        set_name("RectangleWidget" + std::to_string(m_widgetid));
        set_boxtype(Theme::boxtype::fill);
    }

    explicit RectangleWidget(Frame& parent, const Rect& rect = {})
        : RectangleWidget(rect)
    {
        parent.add(*this);
    }

    virtual void draw(Painter& painter, const Rect&) override;
};

}
}

#endif
