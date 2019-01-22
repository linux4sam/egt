/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/view.h"
#include "egt/painter.h"

using namespace std;

namespace egt
{
inline namespace v1
{

ScrolledView::ScrolledView(const Rect& rect)
    : Frame(rect, widgetmask::NO_BACKGROUND),
      m_slider(0, 100, 0, orientation::HORIZONTAL)
{
    resize_slider();

    // TODO: handle slider events for moving the view
}

int ScrolledView::handle(eventid event)
{
    auto mouse = m_mouse.handle(event);
    switch (mouse)
    {
    case Swipe::mouse_event::done:
    case Swipe::mouse_event::none:
        break;
    case Swipe::mouse_event::start:
        m_mouse.start(m_offset);
        return 1;
    case Swipe::mouse_event::drag:
        if (m_orientation == orientation::HORIZONTAL)
        {
            auto diff = from_screen(event_mouse()).x - m_mouse.mouse_start().x;
            set_position(m_mouse.start_value() + diff / 2);
        }
        else
        {
            auto diff = from_screen(event_mouse()).y - m_mouse.mouse_start().y;
            set_position(m_mouse.start_value() + diff / 2);
        }
        return 1;
    case Swipe::mouse_event::click:
        break;
    }

    auto ret = Frame::handle(event);
    return ret;
}

void ScrolledView::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    Painter::AutoSaveRestore sr(painter);
    auto cr = painter.context();

    // change the origin to the offset
    if (m_orientation == orientation::HORIZONTAL)
        cairo_translate(cr.get(), m_offset, 0);
    else
        cairo_translate(cr.get(), 0, m_offset);

    Rect r = box();
    if (m_orientation == orientation::HORIZONTAL)
        r.x -= m_offset;
    else
        r.y -= m_offset;

    Frame::draw(painter, r);

    Rect super = super_rect();
    bool draw_slider = false;

    if (m_orientation == orientation::HORIZONTAL)
        draw_slider = super.w > size().w;
    else
        draw_slider = super.h > size().h;

    if (draw_slider)
        if (Rect::intersect(m_slider.box(), r))
            m_slider.draw(painter, Rect::intersection(m_slider.box(), r));
}

auto SLIDER_DIM = 20;

void ScrolledView::resize(const Size& size)
{
    Frame::resize(size);
    resize_slider();
}

void ScrolledView::resize_slider()
{
    auto b = box();
    if (m_orientation == orientation::HORIZONTAL)
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
    Rect result = box();
    for (auto& child : m_children)
    {
        result = Rect::merge(result, child->box());
    }
    return result;
}

void ScrolledView::set_position(int offset)
{
    Rect super = super_rect();

    if (offset <= 0 && -offset < super.w)
    {
        if (m_offset != offset)
        {
            m_offset = offset;

            Rect super = super_rect();
            float v;
            if (m_orientation == orientation::HORIZONTAL)
            {
                v = std::abs(m_offset) / static_cast<float>(super.w) * 100.;
            }
            else
            {
                v = std::abs(m_offset) / static_cast<float>(super.h) * 100.;
            }

            m_slider.set_value(v);
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
