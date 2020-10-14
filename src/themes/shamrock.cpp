/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/checkbox.h"
#include "egt/painter.h"
#include "egt/text.h"
#include "egt/themes/shamrock.h"
#include "egt/widget.h"

namespace egt
{
inline namespace v1
{

void ShamrockTheme::init_draw()
{
    Theme::init_draw();
    /// @[RoundButtons]
    // round buttons
    Drawer<Button>::draw([](const Button & widget, Painter & painter, const Rect & rect)
    {
        detail::ignoreparam(rect);

        widget.draw_circle(painter, Palette::ColorId::button_bg, Palette::ColorId::border);

        detail::draw_text(painter,
                          widget.content_area(),
                          widget.text(),
                          widget.font(),
        {TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap},
        widget.text_align(),
        Justification::middle,
        widget.color(Palette::ColorId::button_text));
    });
    /// @[RoundButtons]
}

}
}
