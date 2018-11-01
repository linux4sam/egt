/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/view.h"
#include "mui/painter.h"

using namespace std;

namespace mui
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
            m_start_pos = mouse_position();
            m_start_offset = m_offset;
            return 1;
        case eventid::MOUSE_UP:
            m_moving = false;
            return 1;
        case eventid::MOUSE_MOVE:
            if (m_moving)
            {
                if (m_orientation == Orientation::HORIZONTAL)
                {
                    auto diff = screen_to_frame(mouse_position()).x - m_start_pos.x;
                    position(m_start_offset + diff / 2);
                }
                else
                {
                    auto diff = screen_to_frame(mouse_position()).y - m_start_pos.y;
                    position(m_start_offset + diff / 2);
                }
            }
            break;
        default:
            break;
        }

        return ret;
    }

    #if 0
    static void draw_scrollbar(Painter& painter, Orientation orientation,
                               const Rect& box, int offset, int total)
    {
        painter.draw_gradient_box(box, palette().color(Palette::BORDER),
                                  false,
                                  palette().color(Palette::LIGHT, Palette::GROUP_NORMAL));
    }
    #endif

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

        auto HEIGHT = 5;

        if (m_orientation == Orientation::HORIZONTAL)
        {
            Rect s = box();
            s.y = box().y + box().h - HEIGHT - 2;
            s.h = HEIGHT;

            if (!is_flag_set(widgetmask::NO_BORDER))
            {
                s.x = std::abs(m_offset);
                painter.draw_basic_box(s, palette().color(Palette::BORDER),
                                       palette().color(Palette::BG));
            }

            s.w = float(box().w) / 4.;
            s.x = std::abs(m_offset) + (float(std::abs(m_offset)) / float(box().w) *
                                        (float(box().w) - s.w));

            painter.draw_gradient_box(s, palette().color(Palette::BORDER),
                                      false,
                                      palette().color(Palette::HIGHLIGHT));
        }
        else
        {
            // TODO
        }

    }

    ScrolledView::~ScrolledView()
    {
    }
}
