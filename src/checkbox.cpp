/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
#include "egt/checkbox.h"
#include "egt/painter.h"
#include "egt/theme.h"

using namespace std;

namespace egt
{
inline namespace v1
{

CheckBox::CheckBox(const std::string& text,
                   const Rect& rect)
    : Button(text, rect)
{
    set_boxtype(Theme::boxtype::border);
    set_flag(widgetflag::GRAB_MOUSE);
    palette().set(Palette::BG, Palette::GROUP_ACTIVE, palette().color(Palette::HIGHLIGHT));
}

int CheckBox::handle(eventid event)
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

void CheckBox::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    static const int STANDOFF = 5;

    // image
    Rect r(x() + STANDOFF,
           y() + STANDOFF,
           h() - STANDOFF * 2,
           h() - STANDOFF * 2);

    auto group = disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL;

    if (checked())
    {
        draw_box(painter, r);

        // draw an "X"
        static const int OFFSET = 5;
        auto cr = painter.context();
        painter.set_color(palette().color(Palette::HIGHLIGHT, group));
        cairo_move_to(cr.get(), r.x + OFFSET, r.y + OFFSET);
        cairo_line_to(cr.get(), r.x + r.w - OFFSET, r.y + r.h - OFFSET);
        cairo_move_to(cr.get(), r.x + r.w - OFFSET, r.y + OFFSET);
        cairo_line_to(cr.get(), r.x + OFFSET, r.y + r.h - OFFSET);
        painter.set_line_width(3.0);
        cairo_stroke(cr.get());
    }
    else
    {
        draw_box(painter, r);
    }

    // text
    painter.set_color(palette().color(Palette::TEXT, group));
    painter.draw_text(m_text,
                      box(),
                      alignmask::LEFT | alignmask::CENTER,
                      h());
}

CheckBox::~CheckBox()
{}

ToggleBox::ToggleBox(const Rect& rect)
    : CheckBox("", rect)
{
    set_boxtype(Theme::boxtype::rounded_border);
    palette().set(Palette::BG, Palette::GROUP_ACTIVE, palette().color(Palette::BG));
}

void ToggleBox::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    draw_box(painter);

    auto group = disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL;

    if (checked())
    {
        Rect rect = box();
        rect.w /= 2;
        rect.x += rect.w;
        theme().draw_rounded_gradient_box(painter,
                                          rect,
                                          palette().color(Palette::HIGHLIGHT, group));
    }
    else
    {
        Rect rect = box();
        rect.w /= 2;
        theme().draw_rounded_gradient_box(painter,
                                          rect,
                                          palette().color(Palette::HIGHLIGHT, group));
    }
}

ToggleBox::~ToggleBox()
{}

}
}
