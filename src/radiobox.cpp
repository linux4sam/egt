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
            flag_set(widgetmask::GRAB_MOUSE);
            palette().set(Palette::BG, Palette::GROUP_ACTIVE, palette().color(Palette::HIGHLIGHT));
        }

        int RadioBox::handle(eventid event)
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

        void RadioBox::draw(Painter& painter, const Rect& rect)
        {
            ignoreparam(rect);

            static const int STANDOFF = 5;

            Point center(x() + h() / 2, y() + h() / 2);
            float radius = (h() - STANDOFF * 2) / 2;

            auto pat = shared_cairo_pattern_t(cairo_pattern_create_linear(x() + h() / 2,
                                                                          y(),
                                                                          x() + h() / 2,
                                                                          y() + h()),
                                              cairo_pattern_destroy);
            Color step = palette().color(Palette::BG);
            cairo_pattern_add_color_stop_rgb(pat.get(), 0, step.redf(), step.greenf(), step.bluef());
            step = palette().color(Palette::BG).shade(.1);
            cairo_pattern_add_color_stop_rgb(pat.get(), 0.43, step.redf(), step.greenf(), step.bluef());
            step = palette().color(Palette::BG).shade(.15);
            cairo_pattern_add_color_stop_rgb(pat.get(), 0.5, step.redf(), step.greenf(), step.bluef());
            step = palette().color(Palette::BG).shade(.18);
            cairo_pattern_add_color_stop_rgb(pat.get(), 1.0, step.redf(), step.greenf(), step.bluef());

            painter.circle(Circle(center, radius));
            auto cr = painter.context();
            cairo_set_source(cr.get(), pat.get());
            cairo_fill_preserve(cr.get());
            painter.set_color(palette().color(Palette::BG));
            painter.set_line_width(1.0);
            painter.stroke();

            if (checked())
            {
                painter.set_color(Color::BLACK);
                painter.circle(Circle(center, radius / 2));
                painter.fill();
            }

            // text
            painter.draw_text(m_text,
                              box(),
                              palette().color(Palette::TEXT),
                              alignmask::LEFT | alignmask::CENTER,
                              h());
        }

        RadioBox::~RadioBox()
        {}
    }
}
