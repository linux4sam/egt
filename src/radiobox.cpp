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

    set_flag(widgetflag::GRAB_MOUSE);
    palette().set(Palette::BG, Palette::GROUP_ACTIVE, palette().color(Palette::HIGHLIGHT));
}

int RadioBox::handle(eventid event)
{
    auto ret = Widget::handle(event);

    switch (event)
    {
    case eventid::POINTER_CLICK:
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

    auto group = disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL;

    painter.circle(Circle(center, radius));
    painter.set_color(palette().color(Palette::HIGHLIGHT, group));

    painter.set_line_width(1.0);
    painter.stroke();

    if (checked())
    {
        painter.circle(Circle(center, radius / 2));
        painter.fill();
    }

    // text
    painter.draw_text(m_text,
                      box(),
                      palette().color(Palette::TEXT, group),
                      alignmask::LEFT | alignmask::CENTER,
                      h());
}

RadioBox::~RadioBox()
{}

}
}
