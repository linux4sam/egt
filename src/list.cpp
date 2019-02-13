/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/frame.h"
#include "egt/imagecache.h"
#include "egt/input.h"
#include "egt/list.h"
#include "egt/painter.h"

using namespace std;

namespace egt
{

ListBox::ListBox(const Rect& rect)
    : Frame(rect),
      m_view(rect, orientation::VERTICAL),
      m_sizer(orientation::VERTICAL)
{
    set_name("ListBox" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::borderfill);

    m_view.set_align(alignmask::EXPAND);
    add(&m_view);

    m_sizer.set_align(alignmask::EXPAND);
    m_view.add(&m_sizer);
}

ListBox::ListBox(Frame& parent,
                 const Rect& rect)
    : ListBox(rect)
{
    parent.add(this);
}

ListBox::ListBox(const item_array& items,
                 const Rect& rect)
    : ListBox(rect)
{
    for (auto i : items)
        add_item(i);
}

ListBox::ListBox(Frame& parent,
                 const item_array& items,
                 const Rect& rect)
    : ListBox(items, rect)
{
    parent.add(this);
}

void ListBox::add_item(Widget* widget)
{
    auto ret = m_sizer.add(widget);
    if (ret)
    {
        widget->resize(Size(0, item_height()));
        widget->set_align(alignmask::EXPAND_HORIZONTAL);

        if (m_sizer.count_children() == 1)
        {
            m_selected = 0;
            m_sizer.child_at(m_selected)->set_active(true);
        }
    }
}

void ListBox::remove_item(Widget* widget)
{
    m_sizer.remove(widget);

    if (m_selected >= m_sizer.count_children())
    {
        if (m_sizer.count_children())
            set_select(m_sizer.count_children() - 1);
        else
            m_selected = 0;
    }
}

Rect ListBox::item_rect(uint32_t index) const
{
    Rect r(box());
    r.h = item_height();
    r.y += (r.h * index);
    r.y += m_view.offset();
    return r;
}

int ListBox::handle(eventid event)
{
    auto ret = Frame::handle(event);
    if (ret)
        return ret;

    switch (event)
    {
    case eventid::POINTER_CLICK:
    {
        Point mouse = from_screen(event_mouse());
        for (size_t i = 0; i < m_sizer.count_children(); i++)
        {
            if (Rect::point_inside(mouse, item_rect(i)))
            {
                set_select(i);
                break;
            }
        }

        return 1;
    }
    default:
        break;
    }

    return ret;
}

void ListBox::set_select(uint32_t index)
{
    if (m_selected != index)
    {
        if (index < m_sizer.count_children())
        {
            if (m_sizer.count_children() > m_selected)
                m_sizer.child_at(m_selected)->set_active(false);
            m_selected = index;
            if (m_sizer.count_children() > m_selected)
                m_sizer.child_at(m_selected)->set_active(true);

            damage();
            invoke_handlers(eventid::PROPERTY_CHANGED);
        }
    }
}

Rect ListBox::child_area() const
{
    auto b = box() - Size(Theme::DEFAULT_BORDER_WIDTH * 2, Theme::DEFAULT_BORDER_WIDTH * 2);
    b += Point(Theme::DEFAULT_BORDER_WIDTH, Theme::DEFAULT_BORDER_WIDTH);
    return b;
}

ListBox::~ListBox()
{
}

}
