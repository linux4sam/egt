/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/checkbox.h"
#include "egt/detail/alignment.h"
#include "egt/painter.h"
#include "egt/themes/sky.h"
#include "egt/widget.h"

namespace egt
{
inline namespace v1
{

void SkyTheme::init_draw()
{
    Theme::init_draw();

    Drawer<CheckBox>::draw([](const CheckBox & widget, Painter & painter, const Rect & rect)
    {
        detail::ignoreparam(rect);

        widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

        auto b = widget.content_area();
        auto handle = Rect(b.point(), Size(std::min(b.width(), b.height()), std::min(b.width(), b.height())));
        auto text = Rect(handle.top_right() + Point(widget.padding(), 0), b.size() - Size(handle.size().width(), 0));

        if (widget.checked())
        {
            auto fgborder = widget.theme().default_border();

            widget.theme().draw_box(painter, Theme::FillFlag::blend, handle,
                                    widget.color(Palette::ColorId::button_fg),
                                    widget.color(Palette::ColorId::bg),
                                    fgborder);

            auto shrink = handle;
            shrink.shrink_around_center(handle.width() / 2);
            widget.theme().draw_box(painter, Theme::FillFlag::blend, shrink,
                                    widget.color(Palette::ColorId::button_fg),
                                    widget.color(Palette::ColorId::button_fg));
        }
        else
        {
            widget.theme().draw_box(painter, Theme::FillFlag::blend, handle,
                                    widget.color(Palette::ColorId::button_fg),
                                    widget.color(Palette::ColorId::bg),
                                    widget.theme().default_border());
        }

        // text
        painter.set(widget.color(Palette::ColorId::text));
        painter.set(widget.font());
        auto size = painter.text_size(widget.text());
        Rect target = detail::align_algorithm(size,
                                              text,
                                              widget.text_align());
        painter.draw(target.point());
        painter.draw(widget.text());
    });
}

}
}
