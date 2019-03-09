/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/alignment.h"
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
    flags().set(Widget::flag::grab_mouse);
    palette().set(Palette::ColorId::bg, Palette::GroupId::active, palette().color(Palette::ColorId::highlight));
}

int CheckBox::handle(eventid event)
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

void CheckBox::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    static const int STANDOFF = 5;

    // image
    Rect r(x() + STANDOFF,
           y() + STANDOFF,
           h() - STANDOFF * 2,
           h() - STANDOFF * 2);

    auto group = disabled() ? Palette::GroupId::disabled : Palette::GroupId::normal;

    if (checked())
    {
        draw_box(painter, r);

        // draw an "X"
        static const int OFFSET = 5;
        auto cr = painter.context();
        painter.set(palette().color(Palette::ColorId::highlight, group));
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
    painter.set(palette().color(Palette::ColorId::text, group));
    painter.set(font());

    auto size = text_size(m_text);
    Rect target = detail::align_algorithm(size,
                                          box(),
                                          alignmask::left | alignmask::center,
                                          h());
    painter.draw(target.point());
    painter.draw(m_text);
}

CheckBox::~CheckBox()
{}

ToggleBox::ToggleBox(const Rect& rect)
    : CheckBox("", rect)
{
    set_boxtype(Theme::boxtype::rounded_border);
    palette().set(Palette::ColorId::bg, Palette::GroupId::active, palette().color(Palette::ColorId::bg));
}

void ToggleBox::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    draw_box(painter);

    auto group = disabled() ? Palette::GroupId::disabled : Palette::GroupId::normal;

    if (checked())
    {
        Rect rect = box();
        rect.w /= 2;
        rect.x += rect.w;
        theme().draw_rounded_gradient_box(painter,
                                          rect,
                                          palette().color(Palette::ColorId::highlight, group));
    }
    else
    {
        Rect rect = box();
        rect.w /= 2;
        theme().draw_rounded_gradient_box(painter,
                                          rect,
                                          palette().color(Palette::ColorId::highlight, group));
    }
}

ToggleBox::~ToggleBox()
{}

}
}
