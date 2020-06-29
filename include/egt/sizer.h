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
#include <egt/detail/meta.h>
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
 *
 * @see @ref layout_intro
 */

/**
 * Positions and sizes widgets by Orientation.
 *
 * BoxSizer will position widgets added to it in a horizontal or vertical
 * Orientation. The BoxSizer can be homogeneous, which means all widgets will be
 * given equal space, or not.  Also, space can be specified that should be
 * between each widget.
 *
 * An added Widget's minimum_size_hint() will be used.
 *
 * @ingroup sizers
 */
class EGT_API BoxSizer : public Frame
{
public:

    /**
     * @param[in] orient Vertical or horizontal Orientation.
     * @param[in] justify Justification of child widgets.
     */
    explicit BoxSizer(Orientation orient = Orientation::horizontal,
                      Justification justify = Justification::middle)
        : m_orient(orient),
          m_justify(justify)
    {
        name("BoxSizer" + std::to_string(m_widgetid));
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] orient Vertical or horizontal Orientation.
     * @param[in] justify Justification of child widgets.
     */
    explicit BoxSizer(Frame& parent,
                      Orientation orient = Orientation::horizontal,
                      Justification justify = Justification::middle)
        : BoxSizer(orient, justify)
    {
        parent.add(*this);
    }

    void layout() override;

    /**
     * Get the justify.
     */
    EGT_NODISCARD Justification justify() const { return m_justify; }

    /**
     * Set the justify.
     */
    void justify(Justification justify)
    {
        if (detail::change_if_diff<>(m_justify, justify))
            layout();
    }

    /**
     * Get the Orientation.
     */
    EGT_NODISCARD Orientation orient() const { return m_orient; }

    /**
     * Set the Orientation.
     */
    void orient(Orientation orient)
    {
        if (detail::change_if_diff<>(m_orient, orient))
            layout();
    }

    void serialize(Serializer& serializer) const override;

    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs) override;

protected:

    /// Calculate the super rectangle of all the children
    EGT_NODISCARD Size super_rect() const
    {
        if (orient() == Orientation::flex)
        {
            Rect result = size();
            for (auto& child : m_children)
                result = Rect::merge(result, child->box());

            return result.size();
        }

        DefaultDim width = 0;
        DefaultDim height = 0;

        if (orient() == Orientation::horizontal)
        {
            for (auto& child : m_children)
            {
                if (!child->align().is_set(AlignFlag::expand_horizontal))
                {
                    width += child->box().width();
                    height = std::max(child->box().height(), height);
                }
            }
        }
        else
        {
            for (auto& child : m_children)
            {
                if (!child->align().is_set(AlignFlag::expand_vertical))
                {
                    width = std::max(child->box().width(), width);
                    height += child->box().height();
                }
            }
        }
        width += 2. * moat();
        height += 2. * moat();

        if (align().is_set(AlignFlag::expand_horizontal))
            if (width < box().width())
                width = box().width();

        if (align().is_set(AlignFlag::expand_vertical))
            if (height < box().height())
                height = box().height();

        return {width, height};
    }

    /// @private
    Orientation m_orient{Orientation::horizontal};
    /// @private
    Justification m_justify{Justification::start};
};

/**
 * HorizontalBoxSizer helper variation of BoxSizer.
 *
 * @ingroup sizers
 */
class EGT_API HorizontalBoxSizer : public BoxSizer
{
public:

    /**
     * @param[in] justify Justification of child widgets.
     */
    explicit HorizontalBoxSizer(Justification justify = Justification::middle)
        : BoxSizer(Orientation::horizontal, justify)
    {}

    /**
     * @param[in] parent The parent Frame.
     * @param[in] justify Justification of child widgets.
     */
    explicit HorizontalBoxSizer(Frame& parent, Justification justify = Justification::middle)
        : BoxSizer(parent, Orientation::horizontal, justify)
    {}
};

/**
 * VerticalBoxSizer helper variation of BoxSizer.
 *
 * @ingroup sizers
 */
class EGT_API VerticalBoxSizer : public BoxSizer
{
public:

    /**
     * @param[in] justify Justification of child widgets.
     */
    explicit VerticalBoxSizer(Justification justify = Justification::middle)
        : BoxSizer(Orientation::vertical, justify)
    {}

    /**
     * @param[in] parent The parent Frame.
     * @param[in] justify Justification of child widgets.
     */
    explicit VerticalBoxSizer(Frame& parent, Justification justify = Justification::middle)
        : BoxSizer(parent, Orientation::vertical, justify)
    {}
};

/**
 * FlexBoxSizer helper variation of BoxSizer.
 *
 * @ingroup sizers
 */
class EGT_API FlexBoxSizer : public BoxSizer
{
public:

    /**
     * @param[in] justify Justification of child widgets.
     */
    explicit FlexBoxSizer(Justification justify = Justification::middle)
        : BoxSizer(Orientation::flex, justify)
    {}

    /**
     * @param[in] parent The parent Frame.
     * @param[in] justify Justification of child widgets.
     */
    explicit FlexBoxSizer(Frame& parent, Justification justify = Justification::middle)
        : BoxSizer(parent, Orientation::flex, justify)
    {}
};

}
}

#endif
