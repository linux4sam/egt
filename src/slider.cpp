/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/alignment.h"
#include "egt/detail/math.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/slider.h"
#include "egt/textwidget.h"
#include "egt/frame.h"

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
    set_name("Slider" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::blank);

    flags().set(Widget::flag::grab_mouse);

    slider_flags().set(flag::rectangle_handle);
}

Slider::Slider(int min, int max, int value, orientation orient) noexcept
    : Slider(Rect(), min, max, value, orient)
{
}

Slider::Slider(Frame& parent, const Rect& rect, int min, int max, int value,
               orientation orient) noexcept
    : Slider(rect, min, max, value, orient)
{
    parent.add(*this);
}

Slider::Slider(Frame& parent, int min, int max, int value,
               orientation orient) noexcept
    : Slider(Rect(), min, max, value, orient)
{
    parent.add(*this);
}

int Slider::handle_width() const
{
    auto b = content_area();

    auto width = b.w;
    auto height = b.h;

    if (slider_flags().is_set(flag::show_labels) ||
        slider_flags().is_set(flag::show_label))
    {
        if (m_orient == orientation::horizontal)
            height /= 2;
        else
            width /= 2;
    }

    if (slider_flags().is_set(flag::square_handle) ||
        slider_flags().is_set(flag::round_handle))
    {
        if (m_orient == orientation::horizontal)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width);
    }
    else
    {
        if (m_orient == orientation::horizontal)
            return std::min(width / 6, height) * 2;
        else
            return std::min(height / 6, width);
    }
}

int Slider::handle_height() const
{
    auto b = content_area();

    auto width = b.w;
    auto height = b.h;

    if (slider_flags().is_set(flag::show_labels) ||
        slider_flags().is_set(flag::show_label))
    {
        if (m_orient == orientation::horizontal)
            height /= 2;
        else
            width /= 2;
    }

    if (slider_flags().is_set(flag::square_handle) ||
        slider_flags().is_set(flag::round_handle))
    {
        if (m_orient == orientation::horizontal)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width);
    }
    else
    {
        if (m_orient == orientation::horizontal)
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
    auto b = content_area();
    auto dimw = handle_width();
    auto dimh = handle_height();

    if (m_orient == orientation::horizontal)
    {
        auto xv = b.x + to_offset(value);
        if (slider_flags().is_set(flag::origin_opposite))
            xv = b.x + b.w - to_offset(value) - dimw;

        if (slider_flags().is_set(flag::show_labels) ||
            slider_flags().is_set(flag::show_label))
        {
            return Rect(xv,
                        b.y + b.h / 4 - dimh / 2 + b.h / 2,
                        dimw,
                        dimh);
        }
        else
        {
            return Rect(xv,
                        b.y + b.h / 2 - dimh / 2,
                        dimw,
                        dimh);
        }
    }
    else
    {
        auto yv = b.y + b.h - to_offset(value) - dimh;
        if (slider_flags().is_set(flag::origin_opposite))
            yv = b.y + to_offset(value);

        if (slider_flags().is_set(flag::show_labels) ||
            slider_flags().is_set(flag::show_label))
        {
            return Rect(b.x + b.w / 4 - dimw / 2 + b.w / 2,
                        yv,
                        dimw,
                        dimh);
        }
        else
        {
            return Rect(b.x + b.w / 2 - dimw / 2,
                        yv,
                        dimw,
                        dimh);
        }
    }
}

int Slider::handle(eventid event)
{
    switch (event)
    {
    case eventid::raw_pointer_down:
        set_active(true);
        break;
    case eventid::raw_pointer_up:
        set_active(false);
        if (m_invoke_pending)
        {
            m_invoke_pending = false;
            this->invoke_handlers(eventid::property_changed);
        }
        break;
    case eventid::pointer_drag_start:
        m_start_offset = to_offset(m_value);
        break;
    case eventid::pointer_drag:
        if (m_orient == orientation::horizontal)
        {
            auto diff = event::pointer().point - event::pointer().drag_start;
            set_value(to_value(m_start_offset + diff.x));
        }
        else
        {
            auto diff = event::pointer().point - event::pointer().drag_start;
            set_value(to_value(m_start_offset - diff.y));
        }
        break;
    default:
        break;
    }

    return Widget::handle(event);
}

void Slider::draw_label(Painter& painter, int value)
{
    auto b = content_area();
    auto handle = handle_box(value);

    if (m_orient == orientation::horizontal)
        handle -= Point(0, b.h / 2.);
    else
        handle -= Point(b.w / 2., 0);

    auto text = std::to_string(value);
    auto font = TextWidget::scale_font(handle.size(), text, Font());

    painter.set(color(Palette::ColorId::text).color());
    painter.set(font);

    auto text_size = painter.text_size(text);
    Rect target = detail::align_algorithm(text_size,
                                          handle,
                                          alignmask::center,
                                          5);
    painter.draw(target.point());
    painter.draw(text);
}

void Slider::draw_handle(Painter& painter)
{
    auto handle = handle_box();
    //auto dim = std::min(handle.w, handle.h);

    if (slider_flags().is_set(flag::round_handle))
    {
        theme().draw_circle(painter,
                            Theme::boxtype::blank_rounded,
                            handle,
                            color(Palette::ColorId::border),
                            color(Palette::ColorId::button_bg));
    }
    else
    {
        theme().draw_box(painter,
                         Theme::boxtype::blank_rounded,
                         handle,
                         color(Palette::ColorId::border),
                         color(Palette::ColorId::button_bg));
    }
}

void Slider::draw(Painter& painter, const Rect&)
{
    auto b = content_area();
    auto yp = b.y + b.h / 2.;
    auto xp = b.x + b.w / 2.;

    if (m_orient == orientation::horizontal)
    {
        if (slider_flags().is_set(flag::show_labels) ||
            slider_flags().is_set(flag::show_label))
        {
            yp += b.h / 4.;

            if (slider_flags().is_set(flag::show_label))
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
        if (slider_flags().is_set(flag::show_labels) ||
            slider_flags().is_set(flag::show_label))
        {
            xp += b.w / 4.;

            if (slider_flags().is_set(flag::show_label))
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
    auto b = content_area();
    auto handle = handle_box();

    Point a1;
    Point a2;
    Point b1;
    Point b2;

    if (m_orient == orientation::horizontal)
    {
        a1 = Point(b.x, yp);
        a2 = Point(handle.x, yp);
        b1 = Point(handle.x /*+ handle.w*/, yp);
        b2 = Point(b.x + b.w, yp);

        painter.set_line_width(handle.h / 5.0);
    }
    else
    {
        a1 = Point(xp, b.y + b.h);
        a2 = Point(xp, handle.y /*+ handle.h*/);
        b1 = Point(xp, handle.y);
        b2 = Point(xp, b.y);

        painter.set_line_width(handle.w / 5.0);
    }

    if (slider_flags().is_set(flag::consistent_line))
    {
        painter.set(palette().color(Palette::ColorId::button_fg, Palette::GroupId::disabled).color());
        painter.draw(a1, b2);
        painter.stroke();
    }
    else
    {
        painter.set(color(Palette::ColorId::button_fg).color());
        painter.draw(a1, a2);
        painter.stroke();
        painter.set(palette().color(Palette::ColorId::button_fg, Palette::GroupId::disabled).color());
        painter.draw(b1, b2);
        painter.stroke();
    }
}

}
}
