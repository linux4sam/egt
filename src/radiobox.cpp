/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/alignment.h"
#include "egt/button.h"
#include "egt/radiobox.h"
#include "egt/painter.h"

using namespace std;

namespace egt
{
inline namespace v1
{

RadioBox::RadioBox(const std::string& text,
                   const Rect& rect)
    : Button(text, rect)
{
    set_name("RadioBox" + std::to_string(m_widgetid));

    flags().set(Widget::flag::grab_mouse);
    palette().set(Palette::ColorId::bg, Palette::GroupId::active, palette().color(Palette::ColorId::highlight));
}

int RadioBox::handle(eventid event)
{
    auto ret = Widget::handle(event);

    switch (event)
    {
    case eventid::pointer_click:
        check(!checked());
        return 1;
    default:
        break;
    }

    return ret;
}

void RadioBox::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    static const int STANDOFF = 5;

    Point center(x() + h() / 2, y() + h() / 2);
    float radius = (h() - STANDOFF * 2) / 2;

    auto group = disabled() ? Palette::GroupId::disabled : Palette::GroupId::normal;

    painter.draw(Arc(center, radius, 0.0f, 2 * M_PI));
    painter.set(palette().color(Palette::ColorId::highlight, group));

    painter.set_line_width(1.0);
    painter.stroke();

    if (checked())
    {
        painter.draw(Arc(center, radius / 2, 0.0f, 2 * M_PI));
        painter.fill();
    }

    // text
    painter.set(palette().color(Palette::ColorId::text, group));
    painter.set(font());

    auto size = text_size(m_text);
    Rect target = detail::align_algorithm(size, box(), alignmask::left | alignmask::center, h());

    painter.draw(target.point());
    painter.draw(m_text);
}

}
}
