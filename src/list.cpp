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

ListBox::ListBox(const item_array& items) noexcept
    : ListBox(items, Rect())
{}

ListBox::ListBox(const Rect& rect) noexcept
    : ListBox(item_array(), rect)
{}

ListBox::ListBox(const item_array& items, const Rect& rect) noexcept
    : Frame(rect),
      m_view(std::make_shared<ScrolledView>(*this, ScrolledView::Policy::never)),
      m_sizer(std::make_shared<BoxSizer>(Orientation::vertical, Justification::start))
{
    name("ListBox" + std::to_string(m_widgetid));

    boxtype(Theme::BoxFlag::fill);
    border(theme().default_border());

    m_sizer->align(AlignFlag::expand_horizontal);

    m_view->add(m_sizer);

    auto carea = content_area();
    if (!carea.empty())
    {
        m_view->box(to_child(carea));
        m_sizer->resize(carea.size());
    }

    for (auto& i : items)
        add_item_private(i);
}

ListBox::ListBox(Frame& parent, const item_array& items, const Rect& rect) noexcept
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

    m_sizer->add(widget);

    // automatically select the first item
    if (m_sizer->count_children() == 1)
    {
        m_selected = 0;
        m_sizer->child_at(m_selected)->checked(true);
    }
}

std::shared_ptr<Widget> ListBox::item_at(size_t index) const
{
    return m_sizer->child_at(index);
}

void ListBox::remove_item(Widget* widget)
{
    m_sizer->remove(widget);

    if (m_selected >= static_cast<ssize_t>(m_sizer->count_children()))
    {
        if (m_sizer->count_children())
            selected(m_sizer->count_children() - 1);
        else
            m_selected = -1;
    }
}

void ListBox::handle(Event& event)
{
    switch (event.id())
    {
    case EventId::pointer_click:
    {
        Point pos = display_to_local(event.pointer().point);

        for (size_t i = 0; i < m_sizer->count_children(); i++)
        {
            auto cbox = m_sizer->child_at(i)->box();
            cbox.y(cbox.y() + m_view->offset().y());

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
    if (m_selected != static_cast<ssize_t>(index))
    {
        if (index < m_sizer->count_children())
        {
            if (static_cast<ssize_t>(m_sizer->count_children()) > m_selected)
                m_sizer->child_at(m_selected)->checked(false);
            m_selected = index;
            if (static_cast<ssize_t>(m_sizer->count_children()) > m_selected)
                m_sizer->child_at(m_selected)->checked(true);

            damage();
            on_selected_changed.invoke();
        }
    }
}

void ListBox::clear()
{
    m_sizer->remove_all();
}

}
}
