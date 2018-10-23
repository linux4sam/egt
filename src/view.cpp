/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/view.h"
#include "mui/painter.h"

using namespace std;

namespace mui
{

    ScrolledView::ScrolledView(const Point& point,
                               const Size& size)
        : Frame(point, size)
    {
    }

    int ScrolledView::handle(int event)
    {
        auto ret = Frame::handle(event);
        if (ret)
            return ret;

        switch (event)
        {
        case EVT_MOUSE_DOWN:
            Widget::focus(true);
            m_moving = true;
            m_start_pos = mouse_position();
            m_start_offset = m_offset;
            return 1;
        case EVT_MOUSE_UP:
            m_moving = false;
#if 0
            // if the mouse has not really moved, then it was a select call
            if (m_orientation == Orientation::HORIZONTAL)
            {
                if (std::abs(m_start_pos.x - mouse_position().x) < 20)
                {
                }
            }
#endif
            Widget::focus(false);
            return 1;
        case EVT_MOUSE_MOVE:
            if (m_moving)
            {
                if (m_orientation == Orientation::HORIZONTAL)
                {
                    position(m_start_offset + screen_to_frame(mouse_position()).x - m_start_pos.x);
                }
                else
                {
                    position(m_start_offset + screen_to_frame(mouse_position()).y - m_start_pos.y);
                }
            }
            break;
        }
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
    }

    ScrolledView::~ScrolledView()
    {
    }
}
