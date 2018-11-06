/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "egt/list.h"
#include "egt/painter.h"
#include "egt/frame.h"

namespace egt
{
    void ListBoxItem::draw(Painter& painter, const Rect& rect, bool selected)
    {
        if (selected)
        {
            painter.draw_gradient_box(rect,
                                      global_palette().color(Palette::BORDER),
                                      false,
                                      global_palette().color(Palette::HIGHLIGHT));
        }
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
        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            Point mouse = screen_to_frame(mouse_position());
            for (size_t i = 0; i < m_items.size(); i++)
            {
                if (Rect::point_inside(mouse, item_rect(i)))
                {
                    select(i);
                    break;
                }
            }

            return 1;
        }
        default:
            break;
        }

        return Widget::handle(event);
    }

    void ListBox::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        painter.draw_basic_box(box() /*Rect(x(), y(), w(), ITEM_HEIGHT * m_items.size())*/,
                               palette().color(Palette::BORDER),
                               palette().color(Palette::BG));

        if (m_selected < m_items.size())
        {
            for (size_t i = 0; i < m_items.size(); i++)
            {
                m_items[i]->draw(painter, item_rect(i), m_selected == i);
            }
        }
    }

    void ListBox::select(uint32_t index)
    {
        if (m_selected != index)
        {
            //damage(item_rect(m_selected));
            m_selected = index;
            //damage(item_rect(m_selected));
            damage();
            on_selected(index);
        }
    }

    ListBox::~ListBox()
    {
    }

}
