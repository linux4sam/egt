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

#include <egt/detail/meta.h>
#include <egt/frame.h>
#include <egt/serialize.h>
#include <egt/widget.h>
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
class EGT_API CircleWidget : public Widget
{
public:

    /**
     * @param[in] circle Initial circle of the widget.
     */
    explicit CircleWidget(const Circle& circle = Circle());

    /**
     * @param[in] parent The parent Frame.
     * @param[in] circle Initial circle of the widget.
     */
    explicit CircleWidget(Frame& parent, const Circle& circle = Circle());

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit CircleWidget(Serializer::Properties& props);

    /**
     * Get the radius of the widget.
     */
    EGT_NODISCARD Circle::DimType radius() const
    {
        return m_radius;
    }

    void draw(Painter& painter, const Rect&) override;

    /**
     * Serialize the widget to the specified serializer.
     */
    void serialize(Serializer& serializer) const override;

protected:

    /// @private
    Circle::DimType m_radius{};

private:
    /**
     * Deserialize widget properties.
     */
    void deserialize(Serializer::Properties& props) override;
};

/**
 * Basic Line Widget.
 *
 * @ingroup shapes
 */
class EGT_API LineWidget : public Widget
{
public:

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit LineWidget(const Rect& rect = {})
        : Widget(rect)
    {
        name("LineWidget" + std::to_string(m_widgetid));
        fill_flags().clear();
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit LineWidget(Frame& parent, const Rect& rect = {})
        : LineWidget(rect)
    {
        parent.add(*this);
    }

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit LineWidget(Serializer::Properties& props);

    void draw(Painter& painter, const Rect&) override;

    /**
     * Get the horizontal state of the widget.
     */
    EGT_NODISCARD bool horizontal() const { return m_horizontal; }

    /**
     * Set the horizontal state of the widget.
     */
    void horizontal(bool horizontal)
    {
        if (detail::change_if_diff<>(m_horizontal, horizontal))
            damage();
    }

    /**
     * Serialize the widget to the specified serializer.
     */
    void serialize(Serializer& serializer) const override;

protected:
    /// Horizontal state
    bool m_horizontal{true};

private:
    /**
     * Deserialize widget properties.
     */
    void deserialize(Serializer::Properties& props) override;
};

/**
 * Basic Rectangle Widget.
 *
 * @ingroup shapes
 */
class EGT_API RectangleWidget : public Widget
{
public:

    /**
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit RectangleWidget(const Rect& rect = {})
        : Widget(rect)
    {
        name("RectangleWidget" + std::to_string(m_widgetid));
        fill_flags(Theme::FillFlag::blend);
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Initial rectangle of the widget.
     */
    explicit RectangleWidget(Frame& parent, const Rect& rect = {})
        : RectangleWidget(rect)
    {
        parent.add(*this);
    }

    /**
     * @param[in] props list of widget argument and its properties.
     */
    explicit RectangleWidget(Serializer::Properties& props)
        : Widget(props)
    {
        name("RectangleWidget" + std::to_string(m_widgetid));
        fill_flags(Theme::FillFlag::blend);
    }

    void draw(Painter& painter, const Rect& rect) override;
};

}
}

#endif
