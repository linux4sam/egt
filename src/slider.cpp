/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/slider.h"
#include "egt/painter.h"

using namespace std;

namespace egt
{
inline namespace v1
{

Slider::Slider(const Rect& rect, int min, int max, int value,
               orientation orient) noexcept
    : ValueRangeWidget<int>(rect, min, max, value),
      m_orientation(orient)
{
}

Slider::Slider(int min, int max, int value,
               orientation orient) noexcept
    : Slider(Rect(), min, max, value, orient)
{
}

Slider::Slider(orientation orient) noexcept
    : Slider(Rect(), 0, 100, 0, orient)
{}

int Slider::handle_dim() const
{
    if (m_orientation == orientation::HORIZONTAL)
        return std::min(w() / 6, h());
    else
        return std::min(h() / 6, w());
}

Rect Slider::handle_box() const
{
    if (m_orientation == orientation::HORIZONTAL)
    {
        auto dim = handle_dim();
        return Rect(x() + normalize(m_value),
                    y() + h() / 2 - dim / 2,
                    dim,
                    dim);
    }
    else
    {
        auto dim = handle_dim();
        return Rect(x() + w() / 2 - dim / 2,
                    y() + normalize(m_value),
                    dim,
                    dim);
    }
}

int Slider::handle(eventid event)
{
    auto ret = Widget::handle(event);

    switch (event)
    {
    case eventid::MOUSE_DOWN:
    {
        if (Rect::point_inside(from_screen(event_mouse()), handle_box() - box().point()))
        {
            if (m_orientation == orientation::HORIZONTAL)
                m_moving_offset = from_screen(event_mouse()).x;
            else
                m_moving_offset = from_screen(event_mouse()).y;
            m_start_pos = value();
            set_active(true);
            mouse_grab(this);
            return 1;
        }

        break;
    }
    case eventid::MOUSE_UP:
        set_active(false);
        mouse_grab(nullptr);
        if (m_invoke_pending)
        {
            m_invoke_pending = false;
            this->invoke_handlers(eventid::PROPERTY_CHANGED);
        }
        return 1;
    case eventid::MOUSE_MOVE:
        if (active())
        {
            if (m_orientation == orientation::HORIZONTAL)
            {
                auto diff = from_screen(event_mouse()).x - m_moving_offset;
                set_value(m_start_pos + denormalize(diff));
            }
            else
            {
                auto diff = from_screen(event_mouse()).y - m_moving_offset;
                set_value(m_start_pos + denormalize(diff));
            }
            return 1;
        }
        break;
    default:
        break;
    }

    return ret;
}

void Slider::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    auto dim = handle_dim();
    auto handle = handle_box();

    painter.set_color(palette().color(Palette::HIGHLIGHT));
    painter.set_line_width(dim / 5.0);

    if (m_orientation == orientation::HORIZONTAL)
    {
        // line
        painter.line(Point(x(), y() + h() / 2),
                     Point(handle.x, y() + h() / 2));
        painter.stroke();
        painter.set_color(palette().color(Palette::MID));
        painter.line(Point(handle.x + handle.w, y() + h() / 2),
                     Point(x() + w(), y() + h() / 2));
        painter.stroke();

        // handle
#ifdef ROUND_HANDLE
        painter.set_color(palette().color(Palette::HIGHLIGHT));
        painter.circle(Circle(handle.center(), handle_box().h / 2));
        painter.fill();
#else
        theme().draw_rounded_borderfill_box(painter, handle,
                                            palette().color(Palette::BORDER),
                                            palette().color(Palette::HIGHLIGHT));

#endif
    }
    else
    {
        // line
        painter.line(Point(x() + w() / 2, y() + h()),
                     Point(x() + w() / 2, handle.y + handle.h));
        painter.stroke();
        painter.set_color(palette().color(Palette::MID));
        painter.line(Point(x() + w() / 2, handle.y),
                     Point(x() + w() / 2, y()));
        painter.stroke();

        // handle
#ifdef ROUND_HANDLE
        painter.set_color(palette().color(Palette::HIGHLIGHT));
        painter.circle(Circle(handle.center(), handle_box().w / 2));
        painter.fill();
#else
        theme().draw_rounded_borderfill_box(painter, handle,
                                            palette().color(Palette::BORDER),
                                            palette().color(Palette::HIGHLIGHT));

#endif
    }
}

Slider::~Slider()
{
}

}
}
