/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/alignment.h"
#include "egt/detail/enum.h"
#include "egt/detail/math.h"
#include "egt/detail/serialize.h"
#include "egt/detail/textwidget.h"
#include "egt/frame.h"
#include "egt/painter.h"
#include "egt/slider.h"

namespace egt
{
inline namespace v1
{

Slider::Slider(const Rect& rect, int min, int max, int value,
               Orientation orient) noexcept
    : ValueRangeWidget<int>(rect, min, max, value),
      m_orient(orient)
{
    name("Slider" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);

    flags().set(Widget::Flag::grab_mouse);

    slider_flags().set(SliderFlag::rectangle_handle);
}

Slider::Slider(int min, int max, int value, Orientation orient) noexcept
    : Slider(Rect(), min, max, value, orient)
{
}

Slider::Slider(Frame& parent, const Rect& rect, int min, int max, int value,
               Orientation orient) noexcept
    : Slider(rect, min, max, value, orient)
{
    parent.add(*this);
}

Slider::Slider(Frame& parent, int min, int max, int value,
               Orientation orient) noexcept
    : Slider(Rect(), min, max, value, orient)
{
    parent.add(*this);
}

int Slider::handle_width() const
{
    auto b = content_area();

    auto width = b.width();
    auto height = b.height();

    if (slider_flags().is_set(SliderFlag::show_labels) ||
        slider_flags().is_set(SliderFlag::show_label))
    {
        if (m_orient == Orientation::horizontal)
            height /= 2;
        else
            width /= 2;
    }

    if (slider_flags().is_set(SliderFlag::square_handle) ||
        slider_flags().is_set(SliderFlag::round_handle))
    {
        if (m_orient == Orientation::horizontal)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width);
    }
    else
    {
        if (m_orient == Orientation::horizontal)
            return std::min(width / 6, height) * 2;
        else
            return std::min(height / 6, width);
    }
}

int Slider::handle_height() const
{
    auto b = content_area();

    auto width = b.width();
    auto height = b.height();

    if (slider_flags().is_set(SliderFlag::show_labels) ||
        slider_flags().is_set(SliderFlag::show_label))
    {
        if (m_orient == Orientation::horizontal)
            height /= 2;
        else
            width /= 2;
    }

    if (slider_flags().is_set(SliderFlag::square_handle) ||
        slider_flags().is_set(SliderFlag::round_handle))
    {
        if (m_orient == Orientation::horizontal)
            return std::min(width / 6, height);
        else
            return std::min(height / 6, width);
    }
    else
    {
        if (m_orient == Orientation::horizontal)
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

    if (m_orient == Orientation::horizontal)
    {
        auto xv = b.x() + to_offset(value);
        if (slider_flags().is_set(SliderFlag::origin_opposite))
            xv = b.x() + b.width() - to_offset(value) - dimw;

        if (slider_flags().is_set(SliderFlag::show_labels) ||
            slider_flags().is_set(SliderFlag::show_label))
        {
            return {xv,
                    b.y() + b.height() / 4 - dimh / 2 + b.height() / 2,
                    dimw,
                    dimh};
        }
        else
        {
            return {xv,
                    b.y() + b.height() / 2 - dimh / 2,
                    dimw,
                    dimh};
        }
    }
    else
    {
        auto yv = b.y() + b.height() - to_offset(value) - dimh;
        if (slider_flags().is_set(SliderFlag::origin_opposite))
            yv = b.y() + to_offset(value);

        if (slider_flags().is_set(SliderFlag::show_labels) ||
            slider_flags().is_set(SliderFlag::show_label))
        {
            return {b.x() + b.width() / 4 - dimw / 2 + b.width() / 2,
                    yv,
                    dimw,
                    dimh};
        }
        else
        {
            return {b.x() + b.width() / 2 - dimw / 2,
                    yv,
                    dimw,
                    dimh};
        }
    }
}

void Slider::handle(Event& event)
{
    Widget::handle(event);

    switch (event.id())
    {
    case EventId::raw_pointer_up:
        if (m_invoke_pending)
        {
            m_invoke_pending = false;
            this->on_value_changed.invoke();
        }
        break;
    case EventId::pointer_drag_start:
        m_start_offset = to_offset(m_value);
        break;
    case EventId::pointer_drag:
        if (m_orient == Orientation::horizontal)
        {
            auto diff = event.pointer().point - event.pointer().drag_start;
            value(to_value(m_start_offset + diff.x()));
        }
        else
        {
            auto diff = event.pointer().point - event.pointer().drag_start;
            value(to_value(m_start_offset - diff.y()));
        }
        break;
    default:
        break;
    }
}

void Slider::draw_label(Painter& painter, int value)
{
    auto b = content_area();
    auto handle = handle_box(value);

    if (m_orient == Orientation::horizontal)
        handle -= Point(0, b.height() / 2.);
    else
        handle -= Point(b.width() / 2., 0);

    auto text = std::to_string(value);
    auto f = detail::TextWidget::scale_font(handle.size(), text, font());

    painter.set(color(Palette::ColorId::label_text).color());
    painter.set(f);

    auto text_size = painter.text_size(text);
    Rect target = detail::align_algorithm(text_size,
                                          handle,
                                          AlignFlag::center,
                                          5);
    painter.draw(target.point());
    painter.draw(text);
}

void Slider::draw_handle(Painter& painter)
{
    auto handle = handle_box();

    if (slider_flags().is_set(SliderFlag::round_handle))
    {
        theme().draw_circle(painter,
                            Theme::FillFlag::blend,
                            handle,
                            color(Palette::ColorId::border),
                            color(Palette::ColorId::button_bg),
                            border());
    }
    else
    {
        theme().draw_box(painter,
                         Theme::FillFlag::blend,
                         handle,
                         color(Palette::ColorId::border),
                         color(Palette::ColorId::button_bg),
                         border(),
                         0,
                         border_radius());
    }
}

void Slider::draw(Painter& painter, const Rect&)
{
    auto b = content_area();
    auto yp = b.y() + b.height() / 2.;
    auto xp = b.x() + b.width() / 2.;

    if (m_orient == Orientation::horizontal)
    {
        if (slider_flags().is_set(SliderFlag::show_labels) ||
            slider_flags().is_set(SliderFlag::show_label))
        {
            yp += b.height() / 4.;

            if (slider_flags().is_set(SliderFlag::show_label))
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
        if (slider_flags().is_set(SliderFlag::show_labels) ||
            slider_flags().is_set(SliderFlag::show_label))
        {
            xp += b.width() / 4.;

            if (slider_flags().is_set(SliderFlag::show_label))
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

    if (m_orient == Orientation::horizontal)
    {
        a1 = Point(b.x(), yp);
        a2 = Point(handle.x(), yp);
        b1 = Point(handle.x(), yp);
        b2 = Point(b.x() + b.width(), yp);

        painter.line_width(handle.height() / 5.0);
    }
    else
    {
        a1 = Point(xp, b.y() + b.height());
        a2 = Point(xp, handle.y());
        b1 = Point(xp, handle.y());
        b2 = Point(xp, b.y());

        painter.line_width(handle.width() / 5.0);
    }

    if (slider_flags().is_set(SliderFlag::consistent_line))
    {
        painter.set(color(Palette::ColorId::button_fg, Palette::GroupId::disabled).color());
        painter.draw(a1, b2);
        painter.stroke();
    }
    else
    {
        painter.set(color(Palette::ColorId::button_fg).color());
        painter.draw(a1, a2);
        painter.stroke();
        painter.set(color(Palette::ColorId::button_fg, Palette::GroupId::disabled).color());
        painter.draw(b1, b2);
        painter.stroke();
    }
}

template<>
std::map<Slider::SliderFlag, char const*> detail::EnumStrings<Slider::SliderFlag>::data =
{
    {Slider::SliderFlag::rectangle_handle, "rectangle_handle"},
    {Slider::SliderFlag::square_handle, "square_handle"},
    {Slider::SliderFlag::round_handle, "round_handle"},
    {Slider::SliderFlag::show_labels, "show_labels"},
    {Slider::SliderFlag::show_label, "show_label"},
    {Slider::SliderFlag::origin_opposite, "origin_opposite"},
    {Slider::SliderFlag::consistent_line, "consistent_line"},
};

void Slider::serialize(detail::Serializer& serializer) const
{
    ValueRangeWidget<int>::serialize(serializer);

    serializer.add_property("sliderflags", m_slider_flags.to_string());
    serializer.add_property("orient", detail::enum_to_string(orient()));
    serializer.add_property("min", min());
    serializer.add_property("max", max());
    serializer.add_property("value", value());
}

void Slider::deserialize(const std::string& name, const std::string& value,
                         const std::map<std::string, std::string>& attrs)
{
    if (name == "sliderflags")
        m_slider_flags.from_string(value);
    else if (name == "orient")
        orient(detail::enum_from_string<Orientation>(value));
    else if (name == "min")
        min(std::stoi(value));
    else if (name == "max")
        max(std::stoi(value));
    else if (name == "value")
        this->value(std::stoi(value));
    else
        ValueRangeWidget<int>::deserialize(name, value, attrs);
}

}
}
