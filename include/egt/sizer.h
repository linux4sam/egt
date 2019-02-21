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

#include "egt/detail/alignment.h"
#include <egt/frame.h>
#include <sstream>

namespace egt
{
inline namespace v1
{

/**
 * A sizer that positions and sizes child widgets in a horizontal or vertical path.
 */
class BoxSizer : public Frame
{
public:

    BoxSizer(orientation orient = orientation::horizontal, int spacing = 0,
             int hmargin = 0, int vmargin = 0)
        : Frame(Rect(Point(), Size(2 * hmargin, 2 * vmargin))),
          m_spacing(spacing),
          m_hmargin(hmargin),
          m_vmargin(vmargin),
          m_orient(orient)
    {
        set_name("BoxSizer" + std::to_string(m_widgetid));
        set_boxtype(Theme::boxtype::none);
    }

    virtual void move(const Point& point) override
    {
        Frame::move(point);
        reposition();
    }

    virtual void resize(const Size& size) override
    {
        Frame::resize(size);
        reposition();
    }

    virtual Widget* add(Widget* widget) override
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

        return widget;
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

class OrientationPositioner : public Frame
{
public:

    OrientationPositioner(orientation orient = orientation::horizontal, int spacing = 0)
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

    virtual void move(const Point& point) override
    {
        Frame::move(point);
        reposition();
    }

    virtual void resize(const Size& size) override
    {
        auto forced = Rect::merge(Rect(Point(), size), super_rect());
        Frame::resize(forced.size());
        reposition();
    }

    virtual Widget* add(Widget* widget) override
    {
        auto ret = Frame::add(widget);
        if (ret)
            reposition();
        return ret;
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
 * A positioner that will organize widgets in a horizontal row.
 */
class HorizontalPositioner : public Frame
{
public:

    HorizontalPositioner(const Rect& rect,
                         int spacing = 0, alignmask align = alignmask::center)
        : Frame(rect),
          m_spacing(spacing),
          m_align(align)
    {
        set_boxtype(Theme::boxtype::none);
    }

    virtual void move(const Point& point) override
    {
        Frame::move(point);
        reposition();
    }

    virtual void resize(const Size& size) override
    {
        Frame::resize(size);
        reposition();
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

    virtual ~HorizontalPositioner()
    {}

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
