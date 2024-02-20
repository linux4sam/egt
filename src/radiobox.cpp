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
    : Switch(text, rect)
{
    name("RadioBox" + std::to_string(m_widgetid));
}

RadioBox::RadioBox(Frame& parent,
                   const std::string& text,
                   const Rect& rect) noexcept
    : RadioBox(text, rect)
{
    parent.add(*this);
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

}
}
