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

int Slider::handle_width() const
{
    auto width = w();
    auto height = h();

    if (is_set(flags::SHOW_LABELS) ||
        is_set(flags::SHOW_LABEL))
    {
        if (m_orientation == orientation::HORIZONTAL)
            height /= 2;
        else
            width /= 2;
    }

    if (is_set(flags::SQUARE_HANDLE) ||
        is_set(flags::ROUND_HANDLE))
    {
        if (m_orientation == orientation::HORIZONTAL)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width);
    }
    else
    {
        if (m_orientation == orientation::HORIZONTAL)
            return std::min(width / 6, height) * 2;
        else
            return std::min(height / 6, width);
    }
}

int Slider::handle_height() const
{
    auto width = w();
    auto height = h();

    if (is_set(flags::SHOW_LABELS) ||
        is_set(flags::SHOW_LABEL))
    {
        if (m_orientation == orientation::HORIZONTAL)
            height /= 2;
        else
            width /= 2;
    }

    if (is_set(flags::SQUARE_HANDLE) ||
        is_set(flags::ROUND_HANDLE))
    {
        if (m_orientation == orientation::HORIZONTAL)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width);
    }
    else
    {
        if (m_orientation == orientation::HORIZONTAL)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width) * 2;
    }
}

Rect Slider::handle_box() const
{
    return handle_box(m_value);
}

Rect Slider::handle_box(int value) const
{
    auto dimw = handle_width();
    auto dimh = handle_height();

    if (m_orientation == orientation::HORIZONTAL)
    {
        if (is_set(flags::SHOW_LABELS) ||
            is_set(flags::SHOW_LABEL))
        {
            return Rect(x() + to_offset(value),
                        y() + h() / 4 - dimh / 2 + h() / 2,
                        dimw,
                        dimh);
        }
        else
        {
            return Rect(x() + to_offset(value),
                        y() + h() / 2 - dimh / 2,
                        dimw,
                        dimh);
        }
    }
    else
    {
        if (is_set(flags::SHOW_LABELS) ||
            is_set(flags::SHOW_LABEL))
        {
            return Rect(x() + w() / 4 - dimw / 2 + w() / 2,
                        y() + h() - to_offset(value) - dimh,
                        dimw,
                        dimh);
        }
        else
        {
            return Rect(x() + w() / 2 - dimw / 2,
                        y() + h() - to_offset(value) - dimh,
                        dimw,
                        dimh);
        }
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
            m_start_pos = to_offset(value());
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
                set_value(to_value(m_start_pos + diff));
            }
            else
            {
                auto diff = from_screen(event_mouse()).y - m_moving_offset;
                set_value(to_value(m_start_pos - diff));
            }
            return 1;
        }
        break;
    default:
        break;
    }

    return ret;
}

void Slider::draw_label(Painter& painter, int value)
{
    auto b = handle_box(value);

    if (m_orientation == orientation::HORIZONTAL)
        b -= Point(0, h() / 2.);
    else
        b -= Point(w() / 2., 0);

    auto text = std::to_string(value);

    painter.draw_text(text,
                      b,
                      palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL),
                      alignmask::CENTER);
}

void Slider::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    auto handle = handle_box();

    painter.set_color(palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));

    if (m_orientation == orientation::HORIZONTAL)
    {
        painter.set_line_width(handle.h / 5.0);

        auto yp = y() + h() / 2.;
        if (is_set(flags::SHOW_LABELS) ||
            is_set(flags::SHOW_LABEL))
        {
            yp += h() / 4.;

            if (is_set(flags::SHOW_LABEL))
            {
                draw_label(painter, m_value);
            }
            else
            {
                draw_label(painter, m_min);
                draw_label(painter, m_min + ((m_max - m_min) / 2));
                draw_label(painter, m_max);
            }
        }

        // line
        painter.line(Point(x(), yp),
                     Point(handle.x, yp));
        painter.stroke();
        painter.set_color(palette().color(Palette::MID, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));
        painter.line(Point(handle.x + handle.w, yp),
                     Point(x() + w(), yp));
        painter.stroke();

        // handle
        if (is_set(flags::ROUND_HANDLE))
        {
            painter.set_color(palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));
            painter.circle(Circle(handle.center(), handle.h / 2.));
            painter.fill();
        }
        else
        {
            theme().draw_rounded_borderfill_box(painter, handle,
                                                palette().color(Palette::BORDER, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL),
                                                palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));

        }
    }
    else
    {
        painter.set_line_width(handle.w / 5.0);

        auto xp = x() + w() / 2.;
        if (is_set(flags::SHOW_LABELS) ||
            is_set(flags::SHOW_LABEL))
        {
            xp += w() / 4.;

            if (is_set(flags::SHOW_LABEL))
            {
                draw_label(painter, m_value);
            }
            else
            {
                draw_label(painter, m_min);
                draw_label(painter, m_min + ((m_max - m_min) / 2));
                draw_label(painter, m_max);
            }
        }

        // line
        painter.line(Point(xp, y() + h()),
                     Point(xp, handle.y + handle.h));
        painter.stroke();
        painter.set_color(palette().color(Palette::MID, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));
        painter.line(Point(xp, handle.y),
                     Point(xp, y()));
        painter.stroke();

        // handle
        if (is_set(flags::ROUND_HANDLE))
        {
            painter.set_color(palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));
            painter.circle(Circle(handle.center(), handle.w / 2.));
            painter.fill();
        }
        else
        {
            theme().draw_rounded_borderfill_box(painter, handle,
                                                palette().color(Palette::BORDER, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL),
                                                palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));

        }
    }
}

Slider::~Slider()
{
}

}
}
