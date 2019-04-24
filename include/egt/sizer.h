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
#include <sstream>

namespace egt
{
inline namespace v1
{

/**
 * @brief Positions and sizes widgets by orientation.
 */
class BoxSizer : public Frame
{
public:

    /**
     * @param[in] orient Vertical or horizontal orientation.
     * @param[in] spacing Spacing between positioning.
     * @param[in] hmargin Horizontal margin.
     * @param[in] vmargin Vertical margin.
     */
    explicit BoxSizer(orientation orient = orientation::horizontal, int spacing = 0,
                      int hmargin = 0, int vmargin = 0)
        : Frame(Size(2 * hmargin, 2 * vmargin)),
          m_spacing(spacing),
          m_hmargin(hmargin),
          m_vmargin(vmargin),
          m_orient(orient)
    {
        set_name("BoxSizer" + std::to_string(m_widgetid));
        set_boxtype(Theme::boxtype::none);
    }

    /**
    * @param[in] parent The parent Frame.
     * @param[in] orient Vertical or horizontal orientation.
     * @param[in] spacing Spacing between positioning.
     * @param[in] hmargin Horizontal margin.
     * @param[in] vmargin Vertical margin.
     */
    BoxSizer(Frame& parent, orientation orient = orientation::horizontal, int spacing = 0,
             int hmargin = 0, int vmargin = 0)
        : BoxSizer(orient, spacing, hmargin, vmargin)
    {
        parent.add(*this);
    }

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<BoxSizer>(*this).release());
    }

    virtual void add(Widget& widget) override
    {
        Frame::add(widget);
    }

    virtual void add(const std::shared_ptr<Widget>& widget) override
    {
        Frame::add(widget);

        if (m_orient == orientation::horizontal)
        {
            auto w = box().size().w + widget->box().size().w;
            if (count_children() > 1)
                w += m_spacing;
            auto h = std::max(widget->box().size().h, box().size().h);

            resize(Size(w, h));
        }
        else
        {
            auto w = std::max(widget->box().size().w, box().size().w);
            auto h = box().size().h + widget->box().size().h;
            if (count_children() > 1)
                h += m_spacing;

            resize(Size(w, h));
        }
    }

    virtual void remove(Widget* widget) override
    {
        Frame::remove(widget);
        reposition();
    }

    virtual void layout() override
    {
        Frame::layout();
        reposition();
    }

    /**
     * Reposition all child widgets.
     */
    virtual void reposition()
    {
        if (box().size().empty())
            return;

        Rect bounding = Rect(Point(m_hmargin, m_vmargin),
                             box().size() - Size(2 * m_hmargin, 2 * m_vmargin));

        if (m_orient == orientation::vertical)
        {
            for (auto& child : m_children)
            {
                Rect target = detail::align_algorithm(child->box().size(),
                                                      bounding,
                                                      child->align(),
                                                      child->margin());

                // reposition/resize widget
                child->move(target.point());
                child->resize(target.size());

                bounding += Point(0, target.size().h + m_spacing);
                bounding -= Size(0, target.size().h);
            }

        }
        else
        {
            for (auto& child : m_children)
            {
                Rect target = detail::align_algorithm(child->box().size(),
                                                      bounding,
                                                      child->align(),
                                                      child->margin());

                // reposition/resize widget
                child->move(target.point());
                child->resize(target.size());

                bounding += Point(target.size().w + m_spacing, 0);
                bounding -= Size(target.size().w, 0);
            }
        }

        damage();
    }

    virtual ~BoxSizer()
    {}

protected:
    int m_spacing{0};
    int m_hmargin{0};
    int m_vmargin{0};
    orientation m_orient{orientation::horizontal};
};

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
        set_boxtype(Theme::boxtype::none);
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

        auto target = box().point() + Point(m_spacing, m_spacing);
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
        Frame::layout();
        reposition();
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
    HorizontalPositioner(const Rect& rect,
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
        Frame::layout();
        reposition();
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

}
}

#endif
