/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
#include "egt/detail/alignment.h"
#include "egt/detail/layout.h"
#include "egt/frame.h"
#include "egt/painter.h"
#include "egt/radiobox.h"
#include "egt/serialize.h"

namespace egt
{
inline namespace v1
{

RadioBox::RadioBox(const std::string& text,
                   const Rect& rect) noexcept
    : Button(text, rect)
{
    name("RadioBox" + std::to_string(m_widgetid));

    fill_flags().clear();
    padding(5);
    text_align(AlignFlag::left | AlignFlag::center_vertical);

    grab_mouse(true);
}

RadioBox::RadioBox(Frame& parent,
                   const std::string& text,
                   const Rect& rect) noexcept
    : RadioBox(text, rect)
{
    parent.add(*this);
}

RadioBox::RadioBox(Serializer::Properties& props, bool is_derived) noexcept
    : Button(props, true)
{
    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void RadioBox::handle(Event& event)
{
    // NOLINTNEXTLINE(bugprone-parent-virtual-call)
    Widget::handle(event);

    switch (event.id())
    {
    case EventId::pointer_click:
        checked(!checked());
        break;
    default:
        break;
    }
}

void RadioBox::draw(Painter& painter, const Rect& rect)
{
    Drawer<RadioBox>::draw(*this, painter, rect);
}

void RadioBox::default_draw(const RadioBox& widget, Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    widget.draw_box(painter, Palette::ColorId::label_bg, Palette::ColorId::border);

    auto b = widget.content_area();
    std::vector<detail::LayoutRect> rects;
    DefaultDim circle_diameter;
    Rect handle;

    if (widget.show_label())
    {
        painter.set(widget.font());
        auto text_size = painter.text_size(widget.text());

        Rect text;
        if (widget.radiobox_align().is_set(AlignFlag::right) ||
            widget.radiobox_align().is_set(AlignFlag::left))
        {
            circle_diameter = std::min<DefaultDim>(b.width() - text_size.width(), b.height());
            if (circle_diameter < 0)
                circle_diameter = b.width() * 0.15;

            if (widget.radiobox_align().is_set(AlignFlag::right))
            {
                rects.emplace_back(0,
                                   Rect(0, 0, b.width() - circle_diameter, b.height()),
                                   widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, circle_diameter, circle_diameter),
                                   0, 0, widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::horizontal);

                handle = rects[1].rect + b.point();
                text = rects[0].rect + b.point();
            }
            else
            {
                rects.emplace_back(0,
                                   Rect(0, 0, circle_diameter, circle_diameter),
                                   0, 0, widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, b.width() - circle_diameter, b.height()),
                                   widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::horizontal);

                handle = rects[0].rect + b.point();
                text = rects[1].rect + b.point();
            }
        }
        else
        {
            circle_diameter = std::min<DefaultDim>(b.height() - text_size.height(), b.width());
            if (circle_diameter < 0)
                circle_diameter = b.height() * 0.15;

            if (widget.radiobox_align().is_set(AlignFlag::bottom))
            {
                rects.emplace_back(0,
                                   Rect(0, 0, b.width(), text_size.height()),
                                   widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, circle_diameter, circle_diameter),
                                   0, 0, widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::vertical);

                text = rects[0].rect + b.point();
                handle = rects[1].rect + b.point();
            }
            else if (widget.radiobox_align().is_set(AlignFlag::top))
            {
                rects.emplace_back(0,
                                   Rect(0, 0, circle_diameter, circle_diameter),
                                   0, 0, widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, b.width(), text_size.height()),
                                   widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::vertical);

                handle = rects[0].rect + b.point();
                text = rects[1].rect + b.point();
            }
        }

        painter.draw(Circle(handle.center(),
                            (std::min(handle.width(), handle.height()) - widget.theme().default_border() * 2) / 2.));
        painter.set(widget.color(Palette::ColorId::button_fg));
        painter.line_width(widget.theme().default_border());
        painter.stroke();

        // text
        painter.set(widget.color(Palette::ColorId::label_text));
        Rect target = detail::align_algorithm(text_size,
                                              text,
                                              widget.text_align());
        painter.draw(target.point());
        painter.draw(widget.text());
    }
    else
    {
        auto circle_diameter = std::min<DefaultDim>(b.width(), b.height());
        if (circle_diameter < 0)
            circle_diameter = b.width() * 0.15;

        rects.emplace_back(0,
                           Rect(0, 0, circle_diameter, circle_diameter),
                           0, 0, widget.padding() / 2);

        detail::flex_layout(b, rects, Justification::middle, Orientation::horizontal);

        handle = rects[0].rect + b.point();

        painter.draw(Circle(handle.center(),
                            (std::min(handle.width(), handle.height()) - widget.theme().default_border() * 2) / 2.));
        painter.set(widget.color(Palette::ColorId::button_fg));
        painter.line_width(widget.theme().default_border());
        painter.stroke();
    }

    if (widget.checked())
    {
        painter.set(widget.color(Palette::ColorId::button_fg));
        painter.draw(Circle(handle.center(),
                            (std::min(handle.width(), handle.height()) - widget.theme().default_border() * 2) / 2. / 2.));
        painter.fill();
    }
}

void RadioBox::text(const std::string& text)
{
    if (m_text != text)
    {
        if (text.empty())
            show_label(false);
        else
            show_label(true);
    }

    Button::text(text);
}

Size RadioBox::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        s += Size(s.width() / 2 + 5, 0);
        // NOLINTNEXTLINE(bugprone-parent-virtual-call)
        return s + Widget::min_size_hint();
    }

    // NOLINTNEXTLINE(bugprone-parent-virtual-call)
    return default_size() + Widget::min_size_hint();
}

void RadioBox::serialize(Serializer& serializer) const
{
    Button::serialize(serializer);

    serializer.add_property("show_label", show_label());
    if (!radiobox_align().empty())
        serializer.add_property("radiobox_align", radiobox_align());
}

void RadioBox::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        switch (detail::hash(std::get<0>(p)))
        {
        case detail::hash("show_label"):
            show_label(egt::detail::from_string(std::get<1>(p)));
            break;
        case detail::hash("radiobox_align"):
            radiobox_align(AlignFlags(std::get<1>(p)));
            break;
        default:
            return false;
        }
        return true;
    }), props.end());
}

}
}
