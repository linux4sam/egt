/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/imagecache.h"
#include "egt/frame.h"
#include "egt/input.h"
#include "egt/list.h"
#include "egt/painter.h"

namespace egt
{
inline namespace v1
{

ListBox::ListBox(const ItemArray& items) noexcept
    : ListBox(items, Rect())
{}

ListBox::ListBox(const Rect& rect) noexcept
    : ListBox(ItemArray(), rect)
{}

ListBox::ListBox(const ItemArray& items, const Rect& rect) noexcept
    : Frame(rect),
      m_view(*this, ScrolledView::Policy::never),
      m_sizer(Orientation::vertical, Justification::start)
{
    name("ListBox" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);
    border(theme().default_border());

    m_sizer.align(AlignFlag::expand_horizontal);

    m_view.add(m_sizer);

    auto carea = content_area();
    if (!carea.empty())
    {
        m_view.box(to_child(carea));
        m_sizer.resize(carea.size());
    }

    for (auto& i : items)
        add_item_private(i);
}

ListBox::ListBox(Frame& parent, const ItemArray& items, const Rect& rect) noexcept
    : ListBox(items, rect)
{
    parent.add(*this);
}

void ListBox::add_item(const std::shared_ptr<Widget>& widget)
{
    add_item_private(widget);
}

void ListBox::add_item_private(const std::shared_ptr<Widget>& widget)
{
    widget->align(AlignFlag::expand_horizontal);

    m_sizer.add(widget);

    // automatically select the first item
    if (m_sizer.count_children() == 1)
    {
        m_sizer.child_at(0)->checked(true);
    }

    on_items_changed.invoke();
}

std::shared_ptr<Widget> ListBox::item_at(size_t index) const
{
    return m_sizer.child_at(index);
}

void ListBox::remove_item(Widget* widget)
{
    if (m_sizer.is_child(widget))
    {
        const auto checked = widget->checked();

        m_sizer.remove(widget);

        on_items_changed.invoke();

        if (checked)
        {
            if (m_sizer.count_children())
                selected(m_sizer.count_children() - 1);
        }
    }
}

void ListBox::handle(Event& event)
{
    switch (event.id())
    {
    case EventId::pointer_click:
    {
        Point pos = display_to_local(event.pointer().point);

        for (size_t i = 0; i < m_sizer.count_children(); i++)
        {
            auto cbox = m_sizer.child_at(i)->box();
            cbox.y(cbox.y() + m_view.offset().y());

            if (cbox.intersect(pos))
            {
                selected(i);
                break;
            }
        }

        event.stop();
        break;
    }
    case EventId::raw_pointer_down:
    case EventId::raw_pointer_up:
        return;
    default:
        break;
    }

    Frame::handle(event);
}

void ListBox::selected(size_t index)
{
    if (index < m_sizer.count_children())
    {
        bool changed = false;
        for (size_t i = 0; i < m_sizer.count_children(); i++)
        {
            if (m_sizer.child_at(i)->checked() && i != index)
            {
                m_sizer.child_at(i)->checked(false);
                changed = true;
            }
        }

        m_sizer.child_at(index)->checked(true);

        if (changed)
        {
            damage();
            on_selected_changed.invoke();
        }

        on_selected.invoke(index);
    }
}

ssize_t ListBox::selected() const
{
    for (size_t i = 0; i < m_sizer.count_children(); i++)
    {
        if (m_sizer.child_at(i)->checked())
            return i;
    }

    return -1;
}

void ListBox::clear()
{
    if (m_sizer.count_children())
    {
        m_sizer.remove_all();
        on_items_changed.invoke();
    }
}

void ListBox::scroll_top()
{
    m_view.offset(Point(m_view.offset().x(), 0));
}

void ListBox::scroll_bottom()
{
    m_view.offset(Point(m_view.offset().x(), m_view.offset_max().y()));
}

}
}
