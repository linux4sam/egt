/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/list.h"
#include "mui/painter.h"

namespace mui
{
    static auto ITEM_HEIGHT = 40;

    ListBox::ListBox(const item_array& items,
                     const Rect& rect)
        : Widget(rect),
          m_items(items)
    {}

    Rect ListBox::item_rect(uint32_t index) const
    {
        Rect r(box().x, box().y, box().w, ITEM_HEIGHT);
        r.y += (r.h * index);
        return r;
    }

    int ListBox::handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
        {
            for (size_t i = 0; i < m_items.size(); i++)
            {
                if (Rect::point_inside(screen_to_frame(mouse_position()), item_rect(i)))
                {
                    if (m_selected != i)
                    {
                        m_selected = i;
                        damage();
                        on_selected(i);
                    }

                    break;
                }
            }

            return 1;
        }
        }

        return Widget::handle(event);
    }

    void ListBox::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        painter.draw_basic_box(Rect(x(), y(), w(), ITEM_HEIGHT * m_items.size()),
                               palette().color(Palette::BORDER),
                               palette().color(Palette::BG));

        if (!m_items.empty())
        {
            painter.draw_basic_box(item_rect(m_selected),
                                   palette().color(Palette::BORDER),
                                   palette().color(Palette::HIGHLIGHT));

            painter.set_color(palette().color(Palette::TEXT));
            painter.set_font(m_font);

            for (size_t i = 0; i < m_items.size(); i++)
            {
                painter.draw_text(item_rect(i), m_items[i], alignmask::CENTER);
            }
        }
    }

    void ListBox::select(uint32_t index)
    {
        if (m_selected != index)
        {
            damage(item_rect(m_selected));
            m_selected = index;
            damage(item_rect(m_selected));
            on_selected(index);
        }
    }

    ListBox::~ListBox()
    {
    }

}
