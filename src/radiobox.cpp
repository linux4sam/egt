/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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

    painter.circle(Circle(center, radius));
    painter.set_color(palette().color(Palette::ColorId::highlight, group));

    painter.set_line_width(1.0);
    painter.stroke();

    if (checked())
    {
        painter.circle(Circle(center, radius / 2));
        painter.fill();
    }

    // text
    painter.set_color(palette().color(Palette::ColorId::text, group));
    painter.draw_text(m_text,
                      box(),
                      alignmask::left | alignmask::center,
                      h());
}

RadioBox::~RadioBox()
{}

}
}
