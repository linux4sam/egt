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

namespace egt
{
inline namespace v1
{

/**
 * @defgroup sizers Sizing and Positioning
 * Sizing and positioning widgets.
 */

/**
 * @brief Positions and sizes widgets by orientation.
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

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<BoxSizer>(*this).release());
    }

    virtual void layout() override;

    inline justification justify() const { return m_justify; }

    inline void set_justify(justification justify)
    {
        if (detail::change_if_diff<>(m_justify, justify))
        {
            layout();
        }
    }

    orientation orient() const { return m_orient; }

    virtual ~BoxSizer() noexcept = default;

protected:

    Size super_rect() const
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


#if 1
/**
 * @brief Positions widgets based on an orientation.
 * @deprecated This will be removed.
 */
class OrientationPositioner : public Frame
{
public:

    /**
    * @param[in] orient Vertical or horizontal orientation.
    * @param[in] spacing Spacing between positioning.
    */
    explicit OrientationPositioner(orientation orient = orientation::horizontal,
                                   int spacing = 0) noexcept
        : Frame(Rect()),
          m_spacing(spacing),
          m_orient(orient)
    {
        set_name("OrientationPositioner" + std::to_string(m_widgetid));

        if (m_orient == orientation::horizontal)
        {
            set_align(alignmask::expand_vertical);
        }
        else
        {
            set_align(alignmask::expand_horizontal);
        }
    }

    /**
     * @param[in] parent The parent Frame.
     * @param[in] orient Vertical or horizontal orientation.
     * @param[in] spacing Spacing between positioning.
     */
    explicit OrientationPositioner(Frame& parent,
                                   orientation orient = orientation::horizontal, int spacing = 0) noexcept
        : OrientationPositioner(orient, spacing)
    {
        parent.add(*this);
    }

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<OrientationPositioner>(*this).release());
    }

    virtual void move(const Point& point) override
    {
        Frame::move(point);
        reposition();
    }

    virtual void resize(const Size& size) override
    {
        auto forced = Rect::merge(size, super_rect());
        Frame::resize(forced.size());
        reposition();
    }

    virtual void add(const std::shared_ptr<Widget>& widget) override
    {
        Frame::add(widget);
        reposition();
    }

    virtual void remove(Widget* widget) override
    {
        Frame::remove(widget);
        reposition();
    }

    /**
     * Reposition all child widgets.
     */
    virtual void reposition()
    {
        if (box().size().empty())
            return;

        auto bounding = to_child(content_area());

        auto target = bounding.point() + Point(m_spacing, m_spacing);
        if (m_orient == orientation::horizontal)
        {
            for (auto& child : m_children)
            {
                child->move(target);
                target += Point(child->size().w + m_spacing, 0);
            }
        }
        else
        {
            for (auto& child : m_children)
            {
                child->move(target);
                target += Point(0, child->size().h + m_spacing);
            }
        }

        damage();
    }

    virtual void layout() override
    {
        reposition();
        //Frame::layout();
    }

    Rect super_rect() const
    {
        Rect result;
        for (auto& child : m_children)
            result = Rect::merge(result, child->box());
        return result;
    }

    virtual ~OrientationPositioner()
    {}

protected:
    int m_spacing{0};
    orientation m_orient{orientation::horizontal};
};

/**
 * @brief Positions Widgets horizontally.
 * @deprecated This will be removed.
 */
class HorizontalPositioner : public Frame
{
public:

    /**
    * @param[in] rect Rectangle for the widget.
    * @param[in] spacing Spacing between positioning.
    * @param[in] align Alignment for the child widgets.
    */
    explicit HorizontalPositioner(const Rect& rect,
                                  int spacing = 0, alignmask align = alignmask::center)
        : Frame(rect),
          m_spacing(spacing),
          m_align(align)
    {
        set_boxtype(Theme::boxtype::none);
    }

    /**
    * @param[in] parent The parent Frame.
    * @param[in] rect Rectangle for the widget.
    * @param[in] spacing Spacing between positioning.
    * @param[in] align Alignment for the child widgets.
    */
    HorizontalPositioner(Frame& parent, const Rect& rect,
                         int spacing = 0, alignmask align = alignmask::center)
        : HorizontalPositioner(rect, spacing, align)
    {
        parent.add(*this);
    }

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<HorizontalPositioner>(*this).release());
    }

    /**
     * Reposition all child widgets.
     */
    virtual void reposition()
    {
        // align everything in center
        default_dim_type width = 0;
        for (auto& child : m_children)
            width += child->w() + m_spacing;

        default_dim_type offset = w() / 2 - width / 2;
        for (auto& child : m_children)
        {
            if (child)
            {
                Point p;
                if ((m_align & alignmask::center) == alignmask::center)
                {
                    p.y = y() + (h() / 2) - (child->h() / 2);
                }

                if ((m_align & alignmask::top) == alignmask::top)
                    p.y = y();
                else if ((m_align & alignmask::bottom) == alignmask::bottom)
                    p.y = y() + h() - child->h();

                child->move(Point(x() + offset + m_spacing, p.y));
                offset += (child->w() + m_spacing);
            }
        }

        damage();
    }

    virtual void layout() override
    {
        reposition();
        //Frame::layout();
    }

    virtual ~HorizontalPositioner() = default;

protected:

    /**
     * The spacing to use between widgets.
     */
    int m_spacing{0};

    /**
     * The align mask to control how children widgets are positioned.
     */
    alignmask m_align{alignmask::none};
};
#endif

}
}

#endif
