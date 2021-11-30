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
    text_align(AlignFlag::left | AlignFlag::center);

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
    name("RadioBox" + std::to_string(m_widgetid));

    fill_flags().clear();
    padding(5);
    text_align(AlignFlag::left | AlignFlag::center);

    grab_mouse(true);

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

    painter.set(widget.font());
    auto text_size = painter.text_size(widget.text());

    std::vector<detail::LayoutRect> rects;

    auto circle_diameter = std::min<DefaultDim>(b.width() - text_size.width(), b.height());
    if (circle_diameter < 0)
        circle_diameter = b.width() * 0.15;

    rects.emplace_back(0,
                       Rect(0, 0, circle_diameter, circle_diameter),
                       0, 0, widget.padding() / 2);
    rects.emplace_back(0,
                       Rect(0, 0, text_size.width(), text_size.height()),
                       widget.padding() / 2);

    detail::flex_layout(b, rects, Justification::start, Orientation::horizontal);

    auto handle = rects[0].rect + b.point();
    auto text = rects[1].rect + b.point();

    painter.draw(Circle(handle.center(),
                        (std::min(handle.width(), handle.height()) - widget.theme().default_border() * 2) / 2.));
    painter.set(widget.color(Palette::ColorId::button_fg));
    painter.line_width(widget.theme().default_border());
    painter.stroke();

    if (widget.checked())
    {
        painter.draw(Circle(handle.center(),
                            (std::min(handle.width(), handle.height()) - widget.theme().default_border() * 2) / 2. / 2.));
        painter.fill();
    }

    // text
    painter.set(widget.color(Palette::ColorId::label_text));
    Rect target = detail::align_algorithm(text_size,
                                          text,
                                          widget.text_align());
    painter.draw(target.point());
    painter.draw(widget.text());
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

}
}
