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
      m_hslider(0, 100, 0, Orientation::horizontal),
      m_vslider(0, 100, 0, Orientation::vertical),
      m_horizontal_policy(horizontal_policy),
      m_vertical_policy(vertical_policy)
{
    name("ScrolledView" + std::to_string(m_widgetid));

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
      m_hslider(0, 100, 0, Orientation::horizontal),
      m_vslider(0, 100, 0, Orientation::vertical)
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
    if (!m_canvas)
        return;

    //
    // All children are drawn to the internal m_canvas.  Then, the proper part
    // of the canvas is drawn based on of the m_offset.
    //
    Painter cpainter(m_canvas->context());

    Rect crect = to_child(super_rect());

    Palette::GroupId group = Palette::GroupId::normal;
    if (disabled())
        group = Palette::GroupId::disabled;
    else if (active())
        group = Palette::GroupId::active;

    /*
     * Children are drawn on a canvas which is a cairo surface. This
     * surface will be copied to the composition surface.
     * If the canvas has no background and a child as well, it can overlap the
     * previous drawing. For instance, the child is a Label with no background,
     * the new text will be drawn on the canvas then copied on the composition
     * buffer overlapping the previous text. If there is a background for the
     * canvas, it will 'erase' the previous text.
     * To prevent this situation, we force the background drawing of the
     * ScrolledView even if fill flags are set to none.
     */
    cpainter.set(color(Palette::ColorId::bg, group));
    theme().draw_box(cpainter,
                     Theme::FillFlag::solid,
                     crect /*m_canvas->size()*//*to_child(box())*/,
                     color(Palette::ColorId::border, group),
                     color(Palette::ColorId::bg, group),
                     border(),
                     margin());

    for (auto& child : children())
    {
        if (!child->visible())
            continue;

        // don't draw plane frame as child - this is
        // specifically handled by event loop
        if (child->plane_window())
            continue;

        draw_subordinate(cpainter, crect, child.get());
    }

    // change origin to paint canvas area and sliders

    Painter::AutoSaveRestore sr(painter);

    const auto& origin = point();
    if (origin.x() || origin.y())
    {
        //
        // Origin about to change
        //
        auto cr = painter.context();
        cairo_translate(cr.get(),
                        origin.x(),
                        origin.y());
    }

    // limit to content area
    const auto mrect = to_child(content_area());

    /*
     * We really want a basic copy here. We don't want to take into account the
     * previous drawing.
     */
    cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(painter.context().get(), m_canvas->surface().get(),
                             m_offset.x(), m_offset.y());
    cairo_rectangle(painter.context().get(),
                    mrect.point().x(), mrect.point().y(), mrect.width(), mrect.height());
    painter.fill();

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

    update_scrollable();

    if (hold != hscrollable() || vold != vscrollable())
    {
        resize_sliders();
        damage();
    }

    update_sliders();

    auto s = super_rect().size();

    if (!m_canvas || m_canvas->size() != s)
    {
        m_canvas = std::make_unique<Canvas>(s);
        damage();
    }
}

void ScrolledView::resize_sliders()
{
    if (hscrollable())
    {
        auto b = box();
        b.y(b.y() + b.height() - m_slider_dim);
        b.height(m_slider_dim);

        if (vscrollable())
            b.width(b.width() - m_slider_dim);

        m_hslider.move(to_subordinate(b.point()));
        m_hslider.resize(b.size());
    }

    if (vscrollable())
    {
        auto b = box();
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
    auto result = box();
    for (const auto& child : children())
    {
        result = Rect::merge(result, child->to_parent(child->box()));
    }
    return result;
}

Point ScrolledView::offset_max() const
{
    const auto super = super_rect();
    return Point(super.width() - box().width(),
                 super.height() - box().height()) * -1;
}

void ScrolledView::offset(Point offset)
{
    auto offmax = offset_max();
    if (offset.x() > 0)
        offset.x(0);
    else if (offset.x() < offmax.x())
        offset.x(offmax.x());

    if (offset.y() > 0)
        offset.y(0);
    else if (offset.y() < offmax.y())
        offset.y(offmax.y());

    if (detail::change_if_diff<>(m_offset, offset))
    {
        update_sliders();
        damage();
    }
}

void ScrolledView::update_sliders()
{
    const auto offmax = offset_max();

    if (offmax.x() < 0)
    {
        const auto hslider_value =
            egt::detail::normalize<float>(std::abs(m_offset.x()), 0, -offmax.x(), 0, 100);
        if (!detail::float_equal(m_hslider.value(hslider_value), hslider_value))
            damage();
    }

    if (offmax.y() < 0)
    {
        const auto vslider_value =
            egt::detail::normalize<float>(std::abs(m_offset.y()), 0, -offmax.y(), 0, 100);
        if (!detail::float_equal(m_vslider.value(vslider_value), vslider_value))
            damage();
    }
}

void ScrolledView::handle(Event& event)
{
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

    switch (event.id())
    {
    case EventId::raw_pointer_down:
    case EventId::raw_pointer_up:
    case EventId::raw_pointer_move:
    case EventId::pointer_click:
    case EventId::pointer_dblclick:
    case EventId::pointer_hold:
    case EventId::pointer_drag_start:
    case EventId::pointer_drag:
    case EventId::pointer_drag_stop:
    {
        /*
         * Take into account the offset to get the real position of the pointer
         * in the ScrolledView.
         */
        Point pos = display_to_local(event.pointer().point) - m_offset;

        /*
         * The pointer position must be updated before delegating the
         * event handling to the children.
         */
        const auto saved_point = event.pointer().point;
        event.pointer().point -= DisplayPoint(m_offset);
        auto reset = detail::on_scope_exit([&event, &saved_point]()
        {
            event.pointer().point = saved_point;
        });

        for (auto& child : detail::reverse_iterate(m_subordinates))
        {
            if (child->readonly())
                continue;

            if (child->disabled())
                continue;

            if (!child->visible())
                continue;

            if (child->box().intersect(pos))
            {
                child->handle(event);
                if (event.quit())
                    return;
            }
        }

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
    m_hslider.slider_flags().set({Slider::SliderFlag::rectangle_handle,
                                  Slider::SliderFlag::consistent_line});

    m_vslider.slider_flags().set({Slider::SliderFlag::rectangle_handle,
                                  Slider::SliderFlag::inverted,
                                  Slider::SliderFlag::consistent_line});
}

}
}
