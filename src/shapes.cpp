/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/shapes.h"
#include "egt/painter.h"
#include "egt/theme.h"
#include "egt/frame.h"

using namespace std;

namespace egt
{
inline namespace v1
{

CircleWidget::CircleWidget()
{
    set_name("CircleWidget" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::blank);
}

CircleWidget::CircleWidget(const Circle& circle)
    : m_radius(circle.radius)
{
    set_name("CircleWidget" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::blank);

    resize(circle.rect().size());
    move_to_center(circle.center);
}

CircleWidget::CircleWidget(Frame& parent, const Circle& circle)
    : m_radius(circle.radius)
{
    set_name("CircleWidget" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::blank);

    resize(circle.rect().size());
    move_to_center(circle.center);
    parent.add(*this);
}

void CircleWidget::draw(Painter& painter, const Rect&)
{
    auto b = content_area();

    theme().draw_circle(painter,
                        boxtype(),
                        b,
                        color(Palette::ColorId::border),
                        color(Palette::ColorId::button_bg),
                        border(),
                        margin());
}

void LineWidget::draw(Painter& painter, const Rect&)
{
    auto b = content_area();

    auto width = border();
    if (width <= 0)
        width = 1;

    painter.set(color(Palette::ColorId::border).color());
    painter.set_line_width(width);

    if (m_horizontal)
        painter.draw(b.center() - Point(b.w / 2., 0),
                     b.center() + Point(b.w / 2., 0));
    else
        painter.draw(b.center() - Point(0, b.h / 2.),
                     b.center() + Point(0, b.h / 2.));

    painter.stroke();
}

void RectangleWidget::draw(Painter& painter, const Rect&)
{
    auto b = content_area();

    theme().draw_box(painter,
                     boxtype(),
                     b,
                     color(Palette::ColorId::border),
                     color(Palette::ColorId::button_bg),
                     border(),
                     margin());

}
}
}
