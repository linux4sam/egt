/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/view.h"
#include "egt/painter.h"

using namespace std;

namespace egt
{

    ScrolledView::ScrolledView(const Rect& rect)
        : Frame(rect, widgetmask::NO_BACKGROUND)
    {}

    int ScrolledView::handle(eventid event)
    {
        auto ret = Frame::handle(event);
        if (ret)
            return ret;

        switch (event)
        {
        case eventid::MOUSE_DOWN:
            m_moving = true;
            m_start_pos = event_mouse();
            m_start_offset = m_offset;
            return 1;
        case eventid::MOUSE_UP:
            m_moving = false;
            break;
        case eventid::MOUSE_MOVE:
            if (m_moving)
            {
                if (m_orientation == Orientation::HORIZONTAL)
                {
                    auto diff = from_screen(event_mouse()).x - m_start_pos.x;
                    set_position(m_start_offset + diff / 2);
                }
                else
                {
                    auto diff = from_screen(event_mouse()).y - m_start_pos.y;
                    set_position(m_start_offset + diff / 2);
                }

                return 1;
            }
            break;
        default:
            break;
        }

        return 0;
    }

    void ScrolledView::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        Painter::AutoSaveRestore sr(painter);
        auto cr = painter.context();

        // change the origin to the offset
        if (m_orientation == Orientation::HORIZONTAL)
            cairo_translate(cr.get(), m_offset, 0);
        else
            cairo_translate(cr.get(), 0, m_offset);

        Rect r = box();
        if (m_orientation == Orientation::HORIZONTAL)
            r.x -= m_offset;
        else
            r.y -= m_offset;

        Frame::draw(painter, r);

        if (m_orientation == Orientation::HORIZONTAL)
        {
            auto HEIGHT = 4;

            Rect s = box();
            s.y = box().y + box().h - HEIGHT;
            s.h = HEIGHT;

            s.x = box().x + std::abs(m_offset);
            painter.set_line_width(1);
            painter.set_color(palette().color(Palette::BORDER));
            painter.line(s.bottom_left() - Point(0, HEIGHT / 2), s.bottom_right() - Point(0, HEIGHT / 2));

            Rect super = super_rect();

            s.w = float(box().w) / 4.;
            s.x = box().x + std::abs(m_offset) + (float(std::abs(m_offset)) / float(super.w) *
                                                  (float(box().w) - s.w));

            theme().draw_rounded_gradient_box(painter,
                                              s,
                                              palette().color(Palette::BORDER),
                                              palette().color(Palette::HIGHLIGHT));
        }
        else
        {
            // TODO
        }

    }

    Rect ScrolledView::super_rect() const
    {
        Rect result = box();
        for (auto& child : m_children)
        {
            result = Rect::merge(result, child->box());
        }
        return result;
    }

    ScrolledView::~ScrolledView()
    {
    }
}
