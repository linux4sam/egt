/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/dump.h"
#include "egt/detail/math.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/view.h"

namespace egt
{
inline namespace v1
{
ScrolledView::ScrolledView(Policy horizontal_policy,
                           Policy vertical_policy) noexcept
    : ScrolledView(Rect(), horizontal_policy, vertical_policy)
{
}

ScrolledView::ScrolledView(const Rect& rect,
                           Policy horizontal_policy,
                           Policy vertical_policy) noexcept
    : Frame(rect),
      m_hslider(0, 0, 0, Orientation::horizontal),
      m_vslider(0, 0, 0, Orientation::vertical),
      m_horizontal_policy(horizontal_policy),
      m_vertical_policy(vertical_policy)
{
    name("ScrolledView" + std::to_string(m_widgetid));

    // scrolled views are not transparent by default
    fill_flags(Theme::FillFlag::solid);

    init_sliders();
    resize_sliders();
}

ScrolledView::ScrolledView(Frame& parent, const Rect& rect,
                           Policy horizontal_policy,
                           Policy vertical_policy) noexcept
    : ScrolledView(rect, horizontal_policy, vertical_policy)
{
    parent.add(*this);
}

ScrolledView::ScrolledView(Frame& parent,
                           Policy horizontal_policy,
                           Policy vertical_policy) noexcept
    : ScrolledView(horizontal_policy, vertical_policy)
{
    parent.add(*this);
}

ScrolledView::ScrolledView(Serializer::Properties& props, bool is_derived) noexcept
    : Frame(props, true),
      m_hslider(0, 0, 0, Orientation::horizontal),
      m_vslider(0, 0, 0, Orientation::vertical)
{
    init_sliders();
    resize_sliders();

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void ScrolledView::serialize(Serializer& serializer) const
{
    if (hpolicy() != Policy::as_needed)
        serializer.add_property("horizontal_policy", policy2str(hpolicy()));
    if (vpolicy() != Policy::as_needed)
        serializer.add_property("vertical_policy", policy2str(vpolicy()));
    if (hoffset())
        serializer.add_property("horizontal_offset", hoffset());
    if (voffset())
        serializer.add_property("vertical_offset", voffset());

    Frame::serialize(serializer);
}

void ScrolledView::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](const auto & p)
    {
        const auto& name = std::get<0>(p);
        const auto& value = std::get<1>(p);
        if (name == "horizontal_policy")
            hpolicy(str2policy(value));
        else if (name == "vertical_policy")
            vpolicy(str2policy(value));
        else
            return false;
        return true;
    }), props.end());
}

void ScrolledView::post_deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](const auto & p)
    {
        const auto& name = std::get<0>(p);
        const auto& value = std::get<1>(p);
        if (name == "horizontal_offset")
            hoffset(std::stoi(value));
        else if (name == "vertical_offset")
            voffset(std::stoi(value));
        else
            return false;
        return true;
    }), props.end());
}

void ScrolledView::draw(Painter& painter, const Rect& rect)
{
    // draw the widget box
    draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    // change origin to paint children and sliders

    Painter::AutoSaveRestore sr(painter);

    // Origin about to change
    painter.translate(point());

    // limit to content area
    const auto content = content_area();
    if (content.intersect(rect))
    {
        Painter::AutoSaveRestore sr2(painter);

        painter.translate(m_offset);

        auto r = Rect::intersection(rect, content);
        auto crect = to_child(r) - m_offset;

        for (auto& child : children())
        {
            if (!child->visible())
                continue;

            // don't draw plane frame as child - this is
            // specifically handled by event loop
            if (child->plane_window())
                continue;

            draw_subordinate(painter, crect, child.get());
        }
    }

    auto srect = to_subordinate(rect);
    if (hscrollable())
        m_hslider.draw(painter, srect);
    if (vscrollable())
        m_vslider.draw(painter, srect);
}

void ScrolledView::resize(const Size& size)
{
    Frame::resize(size);
    resize_sliders();
}

void ScrolledView::layout()
{
    Frame::layout();

    if (!visible())
        return;

    // we cannot layout with no space
    if (size().empty())
        return;

    if (m_in_layout)
        return;

    m_in_layout = true;
    auto reset = detail::on_scope_exit([this]() { m_in_layout = false; });

    bool hold = hscrollable();
    bool vold = vscrollable();

    const auto super = super_rect();
    m_offset_range = to_child(super - box().size());
    // negate the offset range.
    m_offset_range.point(m_offset_range.bottom_right() * -1);

    update_scrollable();

    if (hold != hscrollable() || vold != vscrollable())
    {
        resize_sliders();
        damage();
    }

    update_sliders();
}

void ScrolledView::resize_sliders()
{
    // Almost the same as the content area, except we ignore the padding.
    auto m = border() + margin();
    auto c = box();
    c += Point(m, m);
    c -= Size(2 * m, 2 * m);
    if (c.empty())
        c.size(Size());

    if (hscrollable())
    {
        auto b = c;
        b.y(b.y() + b.height() - m_slider_dim);
        b.height(m_slider_dim);

        if (vscrollable())
            b.width(b.width() - m_slider_dim);

        m_hslider.move(to_subordinate(b.point()));
        m_hslider.resize(b.size());
    }

    if (vscrollable())
    {
        auto b = c;
        b.x(b.x() + b.width() - m_slider_dim);
        b.width(m_slider_dim);

        if (hscrollable())
            b.height(b.height() - m_slider_dim);

        m_vslider.move(to_subordinate(b.point()));
        m_vslider.resize(b.size());
    }
}

Rect ScrolledView::super_rect() const
{
    // compute the "super content area" first.
    auto result = content_area();
    for (const auto& child : children())
    {
        result = Rect::merge(result, child->to_parent(child->box()));
    }

    // then add the moat around after, so all widgets can be entirely drawn.
    const auto delta = moat();
    result -= Point(delta, delta);
    result += Size(2 * delta, 2 * delta);
    return result;
}

void ScrolledView::offset(Point offset)
{
    m_hslider.value(offset.x());
    m_vslider.value(offset.y());
}

void ScrolledView::update_sliders()
{
    const auto offmin = offset_min();
    const auto offmax = offset_max();

    m_hslider.starting(offmin.x());
    m_hslider.ending(offmax.x());

    m_vslider.starting(offmin.y());
    m_vslider.ending(offmax.y());
}

void ScrolledView::handle(Event& event)
{
    Frame::handle(event);

    switch (event.id())
    {
    case EventId::pointer_drag_start:
        m_start_offset = m_offset;
        break;
    case EventId::pointer_drag:
    {
        auto diff = event.pointer().point -
                    event.pointer().drag_start;
        offset(m_start_offset + Point(diff.x(), diff.y()));
        break;
    }
    default:
        break;
    }
}

std::string ScrolledView::policy2str(Policy policy)
{
    switch (policy)
    {
    default:
    case Policy::as_needed:
        return "as_needed";
    case Policy::never:
        return "never";
    case Policy::always:
        return "always";
    }

    return "as_needed";
}

ScrolledView::Policy ScrolledView::str2policy(const std::string& str)
{
    if (str == "never")
        return Policy::never;
    if (str == "always")
        return Policy::always;
    return Policy::as_needed;
}

void ScrolledView::init_sliders()
{
    auto redraw_content = [this]()
    {
        m_offset.x(m_hslider.value());
        m_offset.y(m_vslider.value());
        damage();
    };

    m_hslider.slider_flags().set({Slider::SliderFlag::rectangle_handle,
                                  Slider::SliderFlag::consistent_line});
    m_hslider.on_value_changed(redraw_content);

    m_vslider.slider_flags().set({Slider::SliderFlag::rectangle_handle,
                                  Slider::SliderFlag::inverted,
                                  Slider::SliderFlag::consistent_line});
    m_vslider.on_value_changed(redraw_content);
}

Point ScrolledView::point_from_subordinate(const Widget& subordinate) const
{
    auto p = Frame::point_from_subordinate(subordinate);

    auto i = std::find_if(children().begin(), children().end(),
                          [&subordinate](const auto & ptr)
    {
        return ptr.get() == &subordinate;
    });
    if (i != children().end())
        p += m_offset;

    return p;
}

}
}
