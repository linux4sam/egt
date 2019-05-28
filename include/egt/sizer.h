/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SIZER_H
#define EGT_SIZER_H

/**
 * @file
 * @brief Working with sizers.
 */

#include <egt/detail/alignment.h>
#include <egt/frame.h>
#include <memory>
#include <utility>

namespace egt
{
inline namespace v1
{

/**
 * @defgroup sizers Sizing and Positioning
 * Sizing and positioning widgets.
 *
 * These widgets, different from other types of widgets like @ref controls, are
 * used specifically for managing position and size of child widgets as well as
 * themselves.
 */

/**
 * Positions and sizes widgets by orientation.
 *
 * BoxSizer will position widgets added to it in a horizontal or vertical
 * orientation. The BoxSizer can be homogeneous, which means all widgets will be
 * given equal space, or not.  Also, space can be specified that should be
 * between each widget.
 *
 * An added Widget's minimum_size_hint() will be used.
 *
 * @ingroup sizers
 */
class BoxSizer : public Frame
{
public:

    /**
     * @param[in] orient Vertical or horizontal orientation.
     * @param[in] justify Justification of child widgets.
     */
    explicit BoxSizer(orientation orient = orientation::horizontal,
                      justification justify = justification::middle)
        : m_orient(orient),
          m_justify(justify)
    {
        set_name("BoxSizer" + std::to_string(m_widgetid));
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] orient Vertical or horizontal orientation.
     * @param[in] justify Justification of child widgets.
     */
    explicit BoxSizer(Frame& parent,
                      orientation orient = orientation::horizontal,
                      justification justify = justification::middle)
        : BoxSizer(orient, justify)
    {
        parent.add(*this);
    }

    virtual void layout() override;

    /**
     * Get the justify.
     */
    inline justification justify() const { return m_justify; }

    /**
     * Set the justify.
     */
    inline void set_justify(justification justify)
    {
        if (detail::change_if_diff<>(m_justify, justify))
            layout();
    }

    /**
     * Get the orientation.
     */
    orientation orient() const { return m_orient; }

    /**
     * Set the orientation.
     */
    inline void set_orient(orientation orient)
    {
        if (detail::change_if_diff<>(m_orient, orient))
            layout();
    }

    virtual ~BoxSizer() noexcept = default;

protected:

    inline Size super_rect() const
    {
        Rect result = size();
        for (auto& child : m_children)
            result = Rect::merge(result, child->box());
        return result.size();
    }

    orientation m_orient{orientation::horizontal};
    justification m_justify{justification::start};
};

/**
 * HorizontalBoxSizer helper variation of BoxSizer.
 *
 * @ingroup sizers
 */
class HorizontalBoxSizer : public BoxSizer
{
public:

    explicit HorizontalBoxSizer(justification justify = justification::middle)
        : BoxSizer(orientation::horizontal, justify)
    {}
    explicit HorizontalBoxSizer(Frame& parent, justification justify = justification::middle)
        : BoxSizer(parent, orientation::horizontal, justify)
    {}

    virtual ~HorizontalBoxSizer() = default;
};

/**
 * VerticalBoxSizer helper variation of BoxSizer.
 *
 * @ingroup sizers
 */
class VerticalBoxSizer : public BoxSizer
{
public:

    explicit VerticalBoxSizer(justification justify = justification::middle)
        : BoxSizer(orientation::vertical, justify)
    {}

    explicit VerticalBoxSizer(Frame& parent, justification justify = justification::middle)
        : BoxSizer(parent, orientation::vertical, justify)
    {}

    virtual ~VerticalBoxSizer() = default;
};

/**
 * FlexBoxSizer helper variation of BoxSizer.
 *
 * @ingroup sizers
 */
class FlexBoxSizer : public BoxSizer
{
public:

    explicit FlexBoxSizer(justification justify = justification::middle)
        : BoxSizer(orientation::flex, justify)
    {}

    explicit FlexBoxSizer(Frame& parent, justification justify = justification::middle)
        : BoxSizer(parent, orientation::flex, justify)
    {}

    virtual ~FlexBoxSizer() = default;
};

}
}

#endif
