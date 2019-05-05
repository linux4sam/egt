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
ScrolledView::ScrolledView(policy horizontal_policy,
                           policy vertical_policy)
    : ScrolledView(Rect(), horizontal_policy, vertical_policy)
{
}

ScrolledView::ScrolledView(const Rect& rect,
                           policy horizontal_policy,
                           policy vertical_policy)
    : Frame(rect),
      m_hslider(0, 100, 0, orientation::horizontal),
      m_vslider(0, 100, 0, orientation::vertical),
      m_horizontal_policy(horizontal_policy),
      m_vertical_policy(vertical_policy)
{
    set_name("ScrolledView" + std::to_string(m_widgetid));
    set_boxtype(Theme::boxtype::blank);

    m_hslider.slider_flags().set({Slider::flag::rectangle_handle,
                                  Slider::flag::consistent_line});

    m_vslider.slider_flags().set({Slider::flag::rectangle_handle,
                                  Slider::flag::origin_opposite,
                                  Slider::flag::consistent_line});

    resize_slider();
}

ScrolledView::ScrolledView(Frame& parent, const Rect& rect,
                           policy horizontal_policy,
                           policy vertical_policy)
    : ScrolledView(rect, horizontal_policy, vertical_policy)
{
    parent.add(*this);
}

ScrolledView::ScrolledView(Frame& parent,
                           policy horizontal_policy,
                           policy vertical_policy)
    : ScrolledView(horizontal_policy, vertical_policy)
{
    parent.add(*this);
}

void ScrolledView::draw(Painter& painter, const Rect& rect)
{
    if (!m_canvas)
        return;

    //
    // All children are drawn to the internal m_canvas.  Then, the proper part
    // of the canvas is drawn based on of the m_offset.
    //

    Painter cpainter(m_canvas->context());

    Rect crect = to_child(super_rect());

    if (boxtype() != Theme::boxtype::none)
    {
        Palette::GroupId group = Palette::GroupId::normal;
        if (disabled())
            group = Palette::GroupId::disabled;
        else if (active())
            group = Palette::GroupId::active;

        theme().draw_box(cpainter,
                         boxtype(),
                         crect,
                         color(Palette::ColorId::border, group),
                         color(Palette::ColorId::bg, group),
                         border(),
                         margin());
    }

    for (auto& child : m_children)
    {
        if (!child->visible())
            continue;

        // don't draw plane frame as child - this is
        // specifically handled by event loop
        if (child->flags().is_set(Widget::flag::plane_window))
            continue;

        if (Rect::intersect(crect, child->box()))
        {
            // don't give a child a rectangle that is outside of its own box
            const auto r = Rect::intersection(crect, child->box());
            if (r.empty())
                continue;

            {
                // no matter what the child draws, clip the output to only the
                // rectangle we care about updating
                Painter::AutoSaveRestore sr2(cpainter);
                if (!child->flags().is_set(Widget::flag::no_clip))
                {
                    cpainter.draw(r);
                    cpainter.clip();
                }

                experimental::code_timer(false, child->name() + " draw: ", [&]()
                {
                    child->draw(cpainter, r);
                });
            }

            special_child_draw(cpainter, child.get());
        }
    }

    // change origin to paint canvas area and sliders

    Painter::AutoSaveRestore sr(painter);

    Point origin = point();
    if (origin.x || origin.y)
    {
        //
        // Origin about to change
        //
        auto cr = painter.context();
        cairo_translate(cr.get(),
                        origin.x,
                        origin.y);
    }

    //cairo_surface_write_to_png(m_canvas->surface().get(), "canvas.png");

    // limit to content area
    const auto mrect = Rect::intersection(to_child(box()), to_child(content_area()));

    cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(painter.context().get(), m_canvas->surface().get(),
                             m_offset.x, m_offset.y);
    cairo_rectangle(painter.context().get(),
                    mrect.point().x, mrect.point().y, mrect.w, mrect.h);
    painter.fill();

    if (hscrollable())
        m_hslider.draw(painter, rect);
    if (vscrollable())
        m_vslider.draw(painter, rect);
}

/// @todo Hardcoded value
const auto SLIDER_DIM = 10;

Rect ScrolledView::content_area() const
{
    auto b = Frame::content_area();

    if (hscrollable())
        b -= Size(0, SLIDER_DIM);

    if (vscrollable())
        b -= Size(SLIDER_DIM, 0);

    return b;
}

void ScrolledView::resize(const Size& size)
{
    Frame::resize(size);
    resize_slider();
}

void ScrolledView::resize_slider()
{
    if (hscrollable())
    {
        auto b = box();
        b.y = b.y + b.h - SLIDER_DIM;
        b.h = SLIDER_DIM;

        if (vscrollable())
            b.w -= SLIDER_DIM;

        m_hslider.move(b.point() - point());
        m_hslider.resize(b.size());
    }

    if (vscrollable())
    {
        auto b = box();
        b.x = b.x + b.w - SLIDER_DIM;
        b.w = SLIDER_DIM;

        if (hscrollable())
            b.h -= SLIDER_DIM;

        m_vslider.move(b.point() - point());
        m_vslider.resize(b.size());
    }
}

Rect ScrolledView::super_rect() const
{
    Rect result = box();
    for (auto& child : m_children)
    {
        result = Rect::merge(result, child->to_parent(child->box()));
    }
    return result;
}

void ScrolledView::set_offset(Point offset)
{
    if (hscrollable() || vscrollable())
    {
        auto super = super_rect();
        auto offset_max = Point(super.w - content_area().w,
                                super.h - content_area().h);
        if (offset.x > 0)
            offset.x = 0;
        else if (-offset.x > offset_max.x)
            offset.x = -offset_max.x;

        if (offset.y > 0)
            offset.y = 0;
        else if (-offset.y > offset_max.y)
            offset.y = -offset_max.y;

        if (detail::change_if_diff<>(m_offset, offset))
        {
            update_sliders();
            damage();
        }
    }
}

void ScrolledView::update_sliders()
{
    auto super = super_rect();
    auto offset_max = Point(super.w - content_area().w,
                            super.h - content_area().h);

    auto hslider_value =
        egt::detail::normalize<float>(std::abs(m_offset.x), 0, offset_max.x, 0, 100);
    if (m_hslider.set_value(hslider_value) != hslider_value)
        damage();

    auto vslider_value =
        egt::detail::normalize<float>(std::abs(m_offset.y), 0, offset_max.y, 0, 100);
    if (m_vslider.set_value(vslider_value) != vslider_value)
        damage();
}

int ScrolledView::handle(eventid event)
{
#if 1
    switch (event)
    {
    case eventid::pointer_drag_start:
        m_start_offset = m_offset;
        break;
    case eventid::pointer_drag:
    {
        auto diff = event::pointer().point -
                    event::pointer().drag_start;
        set_offset(m_start_offset + Point(diff.x, diff.y));
        break;
    }
    default:
        break;
    }
#endif
    auto ret = Widget::handle(event);

    switch (event)
    {
    case eventid::raw_pointer_down:
    case eventid::raw_pointer_up:
    case eventid::raw_pointer_move:
    case eventid::pointer_click:
    case eventid::pointer_dblclick:
    case eventid::pointer_hold:
    case eventid::pointer_drag_start:
    case eventid::pointer_drag:
    case eventid::pointer_drag_stop:
    {
        for (auto& child : detail::reverse_iterate(m_children))
        {
            if (child->readonly())
                continue;

            if (child->disabled())
                continue;

            if (!child->visible())
                continue;

            Point pos = to_child(from_display(event::pointer().point));

            if (Rect::point_inside(pos, child->box() + m_offset))
            {
                auto cret = child->handle(event);
                if (cret)
                    return cret;
            }
        }

        break;
    }
    default:
        break;
    }

    return ret;
}

}
}
