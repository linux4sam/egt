/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/list.h"
#include "egt/painter.h"
#include "egt/frame.h"
#include "egt/imagecache.h"

namespace egt
{
    void ListBoxItem::draw(Painter& painter, const Rect& rect, bool selected, ListBox& listbox)
    {
        if (selected)
        {
            listbox.theme().draw_rounded_gradient_box(painter, rect,
                                              listbox.palette().color(Palette::BORDER),
                                              listbox.palette().color(Palette::HIGHLIGHT));
        }
    }

    void StringItem::draw(Painter& painter, const Rect& rect, bool selected, ListBox& listbox)
    {
        ListBoxItem::draw(painter, rect, selected, listbox);

        painter.set_color(listbox.palette().color(Palette::TEXT));
        painter.set_font(m_font);
        painter.draw_text(rect, m_text, alignmask::CENTER);
    }

    static auto ITEM_HEIGHT = 40;

    Rect ListBox::item_rect(uint32_t index) const
    {
        Rect r(box());
        r.h = ITEM_HEIGHT;
        r.y += (r.h * index);
        return r;
    }

    int ListBox::handle(eventid event)
    {
        auto ret = Widget::handle(event);

        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            Point mouse = from_screen(event_mouse());
            for (size_t i = 0; i < m_items.size(); i++)
            {
                if (Rect::point_inside(mouse, item_rect(i) - box().point()))
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

    void ListBox::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        draw_box(painter);

        if (m_selected < m_items.size())
        {
            for (size_t i = 0; i < m_items.size(); i++)
            {
                m_items[i]->draw(painter, item_rect(i), m_selected == i, *this);
            }
        }
    }

    void ListBox::set_select(uint32_t index)
    {
        if (m_selected != index)
        {
            m_selected = index;
            damage();
            invoke_handlers(eventid::PROPERTY_CHANGED);
        }
    }

    ListBox::~ListBox()
    {
    }

}
