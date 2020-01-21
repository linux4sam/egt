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
    : Popup(Size(parent.size().width(), 40)),
      m_list(std::make_shared<ListBox>()),
      m_parent(parent)
{
    name("ComboBoxPopup" + std::to_string(m_widgetid));

    m_list->align(AlignFlag::expand);

    add(m_list);

    m_list->on_selected_changed([this, &parent]()
    {
        // ?? how to stop event
        //event.stop();
        parent.selected(m_list->selected());
        hide();
    });
}

void ComboBoxPopup::smart_pos()
{
    auto origin = m_parent.local_to_display(Point());

    auto real = origin + DisplayPoint(0, m_parent.size().height() + POPUP_OFFSET);
    move(Point(real.x(), real.y()));

    if (Application::instance().screen())
    {
        auto ss = Application::instance().screen()->size();
        auto height =
            std::min(static_cast<default_dim_type>((ss.height() - origin.y()) / 40),
                     static_cast<default_dim_type>(m_list->item_count())) * 40;

        // hack because list child area is smaller by this much
        height += 2.0 * 3.0;

        resize(Size(m_parent.size().width(), height));
    }
    else
    {
        resize(Size(m_parent.size().width(), 100));
    }
}

void ComboBoxPopup::show()
{
    m_list->clear();
    for (auto& item : m_parent.m_items)
    {
        auto p = std::make_shared<StringItem>(item);
        m_list->add_item(p);
    }

    if (!m_parent.m_items.empty())
        m_list->selected(m_parent.selected());

    smart_pos();
    Popup::show();
}

void ComboBoxPopup::handle(Event& event)
{
    Popup::handle(event);

    /// @todo How is this widget going to get this event?
    switch (event.id())
    {
    case EventId::pointer_click:
    {
        // if any mouse click happens, hide
        hide();
        break;
    }
    default:
        break;
    }
}

}

ComboBox::ComboBox(const item_array& items) noexcept
    : ComboBox(items, {})
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
    : detail::TextWidget("", rect, AlignFlag::left | AlignFlag::center),
      m_items(items),
      m_popup(new detail::ComboBoxPopup(*this))
{
    name("ComboBox" + std::to_string(m_widgetid));

    boxtype({Theme::BoxFlag::fill, Theme::BoxFlag::border_rounded});
    padding(5);
    border(theme().default_border());

    flags().set(Widget::Flag::grab_mouse);

    // automatically select the first item
    if (!m_items.empty())
        m_selected = 0;
}

ComboBox::ComboBox(Frame& parent, const item_array& items, const Rect& rect) noexcept
    : ComboBox(items, rect)
{
    parent.add(*this);
}

void ComboBox::add_item(const std::string& item)
{
    m_items.push_back(item);

    if (m_items.size() == 1)
        m_selected = 0;
}

bool ComboBox::remove(const std::string& item)
{
    auto it = std::find(m_items.begin(), m_items.end(), item);

    if (it != m_items.end())
    {
        m_items.erase(it);
        return true;
    }
    return false;
}

void ComboBox::parent(Frame* parent)
{
    detail::TextWidget::parent(parent);

    /// @todo unsafe to be using main_window() here
    main_window()->add(m_popup);

    m_popup->special_child_draw_callback(parent->special_child_draw_callback());
}

void ComboBox::handle(Event& event)
{
    Widget::handle(event);

    switch (event.id())
    {
    case EventId::pointer_click:
    {
        Point mouse = display_to_local(event.pointer().point);
        if (local_box().intersect(mouse))
        {
            m_popup->show_modal();
        }

        break;
    }
    default:
        break;
    }
}

void ComboBox::selected(size_t index)
{
    if (m_selected != static_cast<ssize_t>(index))
    {
        if (index < m_items.size())
        {
            m_selected = index;
            damage();
            on_selected_changed.invoke();
        }
    }
}

void ComboBox::resize(const Size& s)
{
    detail::TextWidget::resize(s);

    if (m_popup->visible())
        m_popup->smart_pos();
}

void ComboBox::move(const Point& point)
{
    detail::TextWidget::move(point);

    if (m_popup->visible())
        m_popup->smart_pos();
}

Size ComboBox::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        s *= Size(1, 3);
        s += Size(s.width() / 2 + 5, 0);
        return s + Widget::min_size_hint();
    }

    return DEFAULT_COMBOBOX_SIZE + Widget::min_size_hint();
}

void ComboBox::draw(Painter& painter, const Rect& rect)
{
    Drawer<ComboBox>::draw(*this, painter, rect);
}

void ComboBox::default_draw(ComboBox& widget, Painter& painter, const Rect& /*rect*/)
{
    widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    auto b = widget.content_area();
    auto handle_dim = std::min(b.width(), b.height());
    auto handle = Rect(b.top_right() - Point(handle_dim, 0), Size(handle_dim, handle_dim));
    auto text = Rect(b.point(), b.size() - Size(handle.size().width(), 0));

    // draw a down arrow
    painter.set(widget.color(Palette::ColorId::button_fg).color());
    auto cr = painter.context().get();
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    painter.draw(Point(handle.x(), handle.y() + handle.height() / 3.),
                 Point(handle.x() + handle.width() / 2., handle.y() + (handle.height() / 3. * 2.)));
    painter.draw(Point(handle.x() + handle.width() / 2., handle.y() + (handle.height() / 3. * 2.)),
                 Point(handle.x() + handle.width(), handle.y() + handle.height() / 3.));
    painter.line_width(widget.theme().default_border());
    painter.stroke();

    if (widget.selected() >= 0 && widget.selected() < static_cast<ssize_t>(widget.item_count()))
    {
        // text
        painter.set(widget.color(Palette::ColorId::text).color());
        painter.set(widget.font());
        auto size = painter.text_size(widget.item_at(widget.selected()));
        auto target = detail::align_algorithm(size,
                                              text,
                                              widget.text_align());
        painter.draw(target.point());
        painter.draw(widget.item_at(widget.selected()));
    }
}

}
}
