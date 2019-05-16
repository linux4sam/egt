/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/combo.h"
#include "egt/detail/imagecache.h"
#include "egt/frame.h"
#include "egt/image.h"
#include "egt/painter.h"
#include "egt/window.h"
#include <memory>

namespace egt
{
inline namespace v1
{

static const auto POPUP_OFFSET = 5;
static const auto DEFAULT_COMBOBOX_SIZE = Size(200, 30);

namespace detail
{

ComboBoxPopup::ComboBoxPopup(ComboBox& parent)
    : Popup(Size(parent.size().w, 40)),
      m_list(std::make_shared<ListBox>()),
      m_parent(parent)
{
    set_name("ComboBoxPopup" + std::to_string(m_widgetid));

    for (auto& item : parent.m_items)
    {
        auto p = std::make_shared<StringItem>(item);
        m_list->add_item(p);
    }

    m_list->set_align(alignmask::expand);

    add(m_list);

    m_list->on_event([this, &parent](Event & event)
    {
        event.stop();
        parent.set_select(m_list->selected());
        hide();
    }, {eventid::property_changed});
}

void ComboBoxPopup::smart_pos()
{
    auto origin = m_parent.to_display(m_parent.box().point());

    auto real = origin + DisplayPoint(0, m_parent.size().h + POPUP_OFFSET);
    move(Point(real.x, real.y));

    if (main_screen())
    {
        auto ss = main_screen()->size();
        auto height =
            std::min(static_cast<default_dim_type>((ss.h - origin.y) / 40),
                     static_cast<default_dim_type>(m_list->count())) * 40;

        // hack because list child area is smaller by this much
        height += 2.0 * 3.0;

        resize(Size(m_parent.size().w, height));
    }
    else
    {
        resize(Size(m_parent.size().w, 100));
    }
}

void ComboBoxPopup::show(bool center)
{
    smart_pos();
    Popup::show(center);
}

void ComboBoxPopup::handle(Event& event)
{
    Popup::handle(event);

    /// @todo How is this widget going to get this event?
    switch (event.id())
    {
    case eventid::pointer_click:
    {
        Point mouse = from_display(event.pointer().point);

        if (!Rect::point_inside(mouse, box()))
        {
            // if any mouse click happens outside of us, hide
            hide();
        }

        break;
    }
    default:
        break;
    }
}

}

ComboBox::ComboBox(const item_array& items) noexcept
    : ComboBox(items, Rect())
{
}

ComboBox::ComboBox(const Rect& rect) noexcept
    : ComboBox(item_array(), rect)
{
}

/**
 * @todo ComboBox needs the ability to update list, and this needs to
 * be propagated to ComboBoxPopup, which right now won't because it only
 * happens in constructor.
 */

ComboBox::ComboBox(const item_array& items,
                   const Rect& rect) noexcept
    : TextWidget("", rect, alignmask::left | alignmask::center),
      m_items(items),
      m_popup(new detail::ComboBoxPopup(*this))
{
    set_name("ComboBox" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::blank_rounded);
    set_padding(5);
    set_border(theme().default_border());

    ncflags().set(Widget::flag::grab_mouse);
}

ComboBox::ComboBox(Frame& parent, const item_array& items) noexcept
    : ComboBox(items)
{
    parent.add(*this);
}

ComboBox::ComboBox(Frame& parent, const item_array& items, const Rect& rect) noexcept
    : ComboBox(items, rect)
{
    parent.add(*this);
}

void ComboBox::set_parent(Frame* parent)
{
    TextWidget::set_parent(parent);

    /// @todo unsafe to be using main_window() here
    main_window()->add(m_popup);

    m_popup->set_special_child_draw_callback(parent->special_child_draw_callback());
}

void ComboBox::handle(Event& event)
{
    Widget::handle(event);

    switch (event.id())
    {
    case eventid::pointer_click:
    {
        Point mouse = from_display(event.pointer().point);

        if (Rect::point_inside(mouse, box()))
        {
            m_popup->show_modal();
        }

        break;
    }
    default:
        break;
    }
}

void ComboBox::set_select(size_t index)
{
    if (m_selected != index)
    {
        if (index < m_items.size())
        {
            m_selected = index;
            damage();
            invoke_handlers(eventid::property_changed);
        }
    }
}

void ComboBox::resize(const Size& s)
{
    TextWidget::resize(s);

    if (m_popup->visible())
        m_popup->smart_pos();
}

void ComboBox::move(const Point& point)
{
    TextWidget::move(point);

    if (m_popup->visible())
        m_popup->smart_pos();
}

Size ComboBox::min_size_hint() const
{
    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        s *= Size(1, 3);
        s += Size(s.w / 2 + 5, 0);
        return s + Widget::min_size_hint();
    }

    return DEFAULT_COMBOBOX_SIZE + Widget::min_size_hint();
}

void ComboBox::draw(Painter& painter, const Rect& rect)
{
    Drawer<ComboBox>::draw(*this, painter, rect);
}

void ComboBox::default_draw(ComboBox& widget, Painter& painter, const Rect& rect)
{
    widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    auto b = widget.content_area();
    auto handle_dim = std::min(b.w, b.h);
    auto handle = Rect(b.top_right() - Point(handle_dim, 0), Size(handle_dim, handle_dim));
    auto text = Rect(b.point(), b.size() - Size(handle.size().w, 0));

    // draw a down arrow
    painter.set(widget.color(Palette::ColorId::button_fg).color());
    auto cr = painter.context().get();
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    painter.draw(Point(handle.x, handle.y + handle.h / 3.),
                 Point(handle.x + handle.w / 2., handle.y + (handle.h / 3. * 2.)));
    painter.draw(Point(handle.x + handle.w / 2., handle.y + (handle.h / 3. * 2.)),
                 Point(handle.x + handle.w, handle.y + handle.h / 3.));
    painter.set_line_width(widget.theme().default_border());
    painter.stroke();

    if (widget.selected() < widget.item_count())
    {
        // text
        painter.set(widget.color(Palette::ColorId::text).color());
        painter.set(widget.font());
        auto size = painter.text_size(widget.item(widget.selected()));
        auto target = detail::align_algorithm(size,
                                              text,
                                              widget.text_align());
        painter.draw(target.point());
        painter.draw(widget.item(widget.selected()));
    }
}

}
}
