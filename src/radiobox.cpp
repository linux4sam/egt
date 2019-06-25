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

using namespace std;

namespace egt
{
inline namespace v1
{

RadioBox::RadioBox(const std::string& text,
                   const Rect& rect) noexcept
    : Button(text, rect)
{
    set_name("RadioBox" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::blank);
    set_padding(5);
    set_text_align(alignmask::left | alignmask::center);

    flags().set(Widget::flag::grab_mouse);
}

RadioBox::RadioBox(Frame& parent,
                   const std::string& text,
                   const Rect& rect) noexcept
    : RadioBox(text, rect)
{
    parent.add(*this);
}

void RadioBox::handle(Event& event)
{
    Widget::handle(event);

    switch (event.id())
    {
    case eventid::pointer_click:
        set_checked(!checked());
        break;
    default:
        break;
    }
}

void RadioBox::draw(Painter& painter, const Rect& rect)
{
    Drawer<RadioBox>::draw(*this, painter, rect);
}

void RadioBox::default_draw(RadioBox& widget, Painter& painter, const Rect& rect)
{
    widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    auto b = widget.content_area();

    painter.set(widget.font());
    auto text_size = painter.text_size(widget.text());

    vector<detail::LayoutRect> rects;

    rects.emplace_back(0,
                       Rect(0, 0, std::min(b.width - text_size.width - widget.padding(), b.height), std::min(b.width - text_size.width - widget.padding(), b.height)),
                       0, 0, widget.padding() / 2);
    rects.emplace_back(0,
                       Rect(0, 0, text_size.width, text_size.height),
                       widget.padding() / 2);

    detail::flex_layout(b, rects, justification::start, orientation::horizontal);

    auto handle = rects[0].rect + b.point();
    auto text = rects[1].rect + b.point();

    painter.draw(Circle(handle.center(), std::min(handle.width, handle.height) / 2.));
    painter.set(widget.color(Palette::ColorId::button_fg).color());
    painter.set_line_width(widget.theme().default_border());
    painter.stroke();

    if (widget.checked())
    {
        painter.draw(Circle(handle.center(), std::min(handle.width, handle.height) / 2. / 2.));
        painter.fill();
    }

    // text
    painter.set(widget.color(Palette::ColorId::text).color());
    Rect target = detail::align_algorithm(text_size,
                                          text,
                                          widget.text_align());
    painter.draw(target.point());
    painter.draw(widget.text());
}

Size RadioBox::min_size_hint() const
{
    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        //s *= Size(1, 3);
        s += Size(s.width / 2 + 5, 0);
        return s + Widget::min_size_hint();
    }

    return Size(100, 30) + Widget::min_size_hint();
}

}
}
