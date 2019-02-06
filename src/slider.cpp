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
      m_orient(orient)
{
    flag_set(widgetmask::GRAB_MOUSE);
    m_mouse.on_async_event(std::bind(&Slider::on_mouse_event, this, std::placeholders::_1));

    static auto slider_id = 0;

    ostringstream ss;
    ss << "Slider" << slider_id++;
    set_name(ss.str());
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
        if (m_orient == orientation::HORIZONTAL)
            height /= 2;
        else
            width /= 2;
    }

    if (is_set(flags::SQUARE_HANDLE) ||
        is_set(flags::ROUND_HANDLE))
    {
        if (m_orient == orientation::HORIZONTAL)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width);
    }
    else
    {
        if (m_orient == orientation::HORIZONTAL)
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
        if (m_orient == orientation::HORIZONTAL)
            height /= 2;
        else
            width /= 2;
    }

    if (is_set(flags::SQUARE_HANDLE) ||
        is_set(flags::ROUND_HANDLE))
    {
        if (m_orient == orientation::HORIZONTAL)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width);
    }
    else
    {
        if (m_orient == orientation::HORIZONTAL)
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

    if (m_orient == orientation::HORIZONTAL)
    {
        auto xv = x() + to_offset(value);
        if (is_set(flags::ORIGIN_OPPOSITE))
            xv = x() + w() - to_offset(value) - dimw;

        if (is_set(flags::SHOW_LABELS) ||
            is_set(flags::SHOW_LABEL))
        {
            return Rect(xv,
                        y() + h() / 4 - dimh / 2 + h() / 2,
                        dimw,
                        dimh);
        }
        else
        {
            return Rect(xv,
                        y() + h() / 2 - dimh / 2,
                        dimw,
                        dimh);
        }
    }
    else
    {
        auto yv = y() + h() - to_offset(value) - dimh;
        if (is_set(flags::ORIGIN_OPPOSITE))
            yv = y() + to_offset(value);

        if (is_set(flags::SHOW_LABELS) ||
            is_set(flags::SHOW_LABEL))
        {
            return Rect(x() + w() / 4 - dimw / 2 + w() / 2,
                        yv,
                        dimw,
                        dimh);
        }
        else
        {
            return Rect(x() + w() / 2 - dimw / 2,
                        yv,
                        dimw,
                        dimh);
        }
    }
}

void Slider::on_mouse_event(Swipe::mouse_event event)
{
    switch (event)
    {
    case Swipe::mouse_event::none:
        break;
    case Swipe::mouse_event::start:
        m_mouse.start(to_offset(m_value));
        set_active(true);
        break;
    case Swipe::mouse_event::click:
    case Swipe::mouse_event::long_click:
    case Swipe::mouse_event::drag_done:
        set_active(false);
        if (m_invoke_pending)
        {
            m_invoke_pending = false;
            this->invoke_handlers(eventid::PROPERTY_CHANGED);
        }
        break;
    case Swipe::mouse_event::drag:
        if (m_orient == orientation::HORIZONTAL)
        {
            auto diff = event_mouse() - m_mouse.mouse_start();
            set_value(to_value(m_mouse.start_value() + diff.x));
        }
        else
        {
            auto diff = event_mouse() - m_mouse.mouse_start();
            set_value(to_value(m_mouse.start_value() - diff.y));
        }
        break;
    }
}

int Slider::handle(eventid event)
{
    m_mouse.handle(event);
    return Widget::handle(event);
}

void Slider::draw_label(Painter& painter, int value)
{
    auto b = handle_box(value);

    if (m_orient == orientation::HORIZONTAL)
        b -= Point(0, h() / 2.);
    else
        b -= Point(w() / 2., 0);

    auto text = std::to_string(value);
    auto font = TextWidget::scale_font(b.size(), text, Font());

    painter.draw_text(text,
                      b,
                      palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL),
                      alignmask::CENTER,
                      5,
                      font);
}

void Slider::draw_handle(Painter& painter)
{
    auto handle = handle_box();
    auto dim = std::min(handle.w, handle.h);

    if (is_set(flags::ROUND_HANDLE))
    {
        painter.set_color(palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));
        painter.circle(Circle(handle.center(), dim / 2.));
        painter.fill();
    }
    else
    {
        theme().draw_rounded_fill_box(painter, handle,
                                      palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));

    }
}

void Slider::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    painter.set_color(palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));

    auto yp = y() + h() / 2.;
    auto xp = x() + w() / 2.;

    if (m_orient == orientation::HORIZONTAL)
    {
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
    }
    else
    {
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

    }

    // line
    draw_line(painter, xp, yp);

    // handle
    draw_handle(painter);
}

void Slider::draw_line(Painter& painter, float xp, float yp)
{
    auto handle = handle_box();

    Point a1;
    Point a2;
    Point b1;
    Point b2;

    if (m_orient == orientation::HORIZONTAL)
    {
        a1 = Point(x(), yp);
        a2 = Point(handle.x, yp);
        b1 = Point(handle.x + handle.w, yp);
        b2 = Point(x() + w(), yp);

        painter.set_line_width(handle.h / 5.0);
    }
    else
    {
        a1 = Point(xp, y() + h());
        a2 = Point(xp, handle.y + handle.h);
        b1 = Point(xp, handle.y);
        b2 = Point(xp, y());

        painter.set_line_width(handle.w / 5.0);
    }

    if (is_set(flags::CONSISTENT_LINE))
    {
        painter.set_color(palette().color(Palette::MID, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));
        painter.line(a1, b2);
        painter.stroke();
    }
    else
    {
        painter.set_color(palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));
        painter.line(a1, a2);
        painter.stroke();
        painter.set_color(palette().color(Palette::MID, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));
        painter.line(b1, b2);
        painter.stroke();
    }
}

Slider::~Slider()
{
}

}
}
