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
    set_boxtype(Theme::boxtype::rounded_gradient);
    flag_set(widgetmask::GRAB_MOUSE);
    palette().set(Palette::BG, Palette::GROUP_ACTIVE, palette().color(Palette::HIGHLIGHT));
}

int CheckBox::handle(eventid event)
{
    auto ret = Widget::handle(event);

    switch (event)
    {
    case eventid::MOUSE_DOWN:
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

    if (checked())
    {
        draw_box(painter, r);

        // draw an "X"
        static const int OFFSET = 5;
        auto cr = painter.context();
        painter.set_color(palette().color(Palette::DARK));
        cairo_move_to(cr.get(), r.x + OFFSET, r.y + OFFSET);
        cairo_line_to(cr.get(), r.x + r.w - OFFSET, r.y + r.h - OFFSET);
        cairo_move_to(cr.get(), r.x + r.w - OFFSET, r.y + OFFSET);
        cairo_line_to(cr.get(), r.x + OFFSET, r.y + r.h - OFFSET);
        painter.set_line_width(2.0);
        cairo_stroke(cr.get());
    }
    else
    {
        draw_box(painter, r);
    }

    // text
    painter.draw_text(m_text,
                      box(),
                      palette().color(Palette::TEXT),
                      alignmask::LEFT | alignmask::CENTER,
                      h());
}

CheckBox::~CheckBox()
{}

SlidingCheckBox::SlidingCheckBox(const Rect& rect)
    : CheckBox("", rect)
{
    set_boxtype(Theme::boxtype::rounded_border);
    palette().set(Palette::BG, Palette::GROUP_ACTIVE, palette().color(Palette::BG));
}

void SlidingCheckBox::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    draw_box(painter);

    if (checked())
    {
        Rect rect = box();
        rect.w /= 2;
        rect.x += rect.w;
        theme().draw_rounded_gradient_box(painter,
                                          rect,
                                          palette().color(Palette::BORDER),
                                          palette().color(Palette::HIGHLIGHT));
    }
    else
    {
        Rect rect = box();
        rect.w /= 2;
        theme().draw_rounded_gradient_box(painter,
                                          rect,
                                          palette().color(Palette::BORDER),
                                          palette().color(Palette::MID));
    }
}

SlidingCheckBox::~SlidingCheckBox()
{}
}
}
