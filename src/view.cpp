/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/math.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/view.h"
#include <sstream>

using namespace std;

namespace egt
{
inline namespace v1
{

// orientation to size dimension
static inline int o2d(orientation o, const Size& size)
{
    return (o == orientation::HORIZONTAL) ? size.w : size.h;
}

// orientation to size dimension
static inline int o2d(orientation o, const Rect& rect)
{
    return o2d(o, rect.size());
}

// orientation to point axis
static inline int o2p(orientation o, const Point& point)
{
    return (o == orientation::HORIZONTAL) ? point.x : point.y;
}

ScrolledView::ScrolledView(const Rect& rect, orientation orient)
    : Frame(rect, widgetmask::NO_BACKGROUND),
      m_slider(0, 100, 0, orient),
      m_orient(orient)
{
    static auto scrolledview_id = 0;
    std::ostringstream ss;
    ss << "ScrolledView" << scrolledview_id++;
    set_name(ss.str());

    if (orient == orientation::VERTICAL)
        m_slider.slider_flags(Slider::flags::RECTANGLE_HANDLE |
                              Slider::flags::ORIGIN_OPPOSITE |
                              Slider::flags::CONSISTENT_LINE);
    else
        m_slider.slider_flags(Slider::flags::RECTANGLE_HANDLE |
                              Slider::flags::CONSISTENT_LINE);

    resize_slider();

    // TODO: handle slider events for moving the view
}

ScrolledView::ScrolledView(orientation orient)
    : ScrolledView(Rect(), orient)
{
}

int ScrolledView::handle(eventid event)
{
    switch (event)
    {
    case eventid::POINTER_DRAG_START:
        m_start_offset = m_offset;
        break;
    case eventid::POINTER_DRAG:
    {
        auto diff = o2p(m_orient, event_mouse()) -
                    o2p(m_orient, event_mouse_drag_start());
        set_offset(m_start_offset + diff);
        break;
    }
    default:
        break;
    }

    return Frame::handle(event);
}

bool ScrolledView::scrollable() const
{
    Rect super = super_rect();
    return o2d(m_orient, super) > o2d(m_orient, size());
}

void ScrolledView::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    Painter::AutoSaveRestore sr(painter);
    auto cr = painter.context();

    // change the origin to the offset
    if (m_orient == orientation::HORIZONTAL)
        cairo_translate(cr.get(), m_offset, 0);
    else
        cairo_translate(cr.get(), 0, m_offset);

    Rect r = box();
    if (m_orient == orientation::HORIZONTAL)
        r.x -= m_offset;
    else
        r.y -= m_offset;

    Frame::draw(painter, r);

    if (scrollable())
    {
        if (Rect::intersect(m_slider.box(), r))
            m_slider.draw(painter, Rect::intersection(m_slider.box(), r));
    }
}

auto SLIDER_DIM = 10;

Rect ScrolledView::child_area() const
{
    // reserve the slider area to keep away from children
    if (!scrollable())
        return box();

    if (m_orient == orientation::HORIZONTAL)
        return box() - Size(0, SLIDER_DIM);

    return box() - Size(SLIDER_DIM, 0);
}

void ScrolledView::resize(const Size& size)
{
    Frame::resize(size);
    resize_slider();
}

void ScrolledView::resize_slider()
{
    auto b = box();
    if (m_orient == orientation::HORIZONTAL)
    {
        b.x += std::abs(m_offset);
        b.y = b.y + b.h - SLIDER_DIM;
        b.h = SLIDER_DIM;
    }
    else
    {
        b.x = b.x + b.w - SLIDER_DIM;
        b.y += std::abs(m_offset);
        b.w = SLIDER_DIM;
    }

    m_slider.move(b.point());
    m_slider.resize(b.size());
}

Rect ScrolledView::super_rect() const
{
    Rect result;
    for (auto& child : m_children)
    {
        result = Rect::merge(result, child->box());
    }
    return result;
}

void ScrolledView::set_offset(int offset)
{
    if (scrollable())
    {
        Rect super = super_rect();
        auto offset_max = o2d(m_orient, super) - o2d(m_orient, box());
        if (offset > 0)
            offset = 0;
        else if (-offset > offset_max)
            offset = -offset_max;

        if (m_offset != offset)
        {
            m_offset = offset;

            auto slider_value =
                egt::detail::normalize<float>(std::abs(m_offset), 0, offset_max,
                                              0, 100);

            m_slider.set_value(slider_value);
            resize_slider();

            damage();
        }
    }
}

ScrolledView::~ScrolledView()
{
}

}
}
