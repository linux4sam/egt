/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
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

void RadioBox::draw_switch(Painter& painter, const Rect& handle) const
{
    if (switch_image(checked()))
    {
        Switch::draw_switch(painter, handle);
        return;
    }

    painter.draw(Circle(handle.center(),
                        (std::min(handle.width(), handle.height()) - theme().default_border() * 2) / 2.));
    painter.set(color(Palette::ColorId::button_fg));
    painter.line_width(theme().default_border());
    painter.stroke();

    if (checked())
    {
        painter.draw(Circle(handle.center(),
                            (std::min(handle.width(), handle.height()) - theme().default_border() * 2) / 2. / 2.));
        painter.fill();
    }
}

}
}
