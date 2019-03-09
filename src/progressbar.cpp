/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/alignment.h"
#include "egt/detail/math.h"
#include "egt/painter.h"
#include "egt/progressbar.h"
#include "egt/textwidget.h"
#include <string>

using namespace std;

namespace egt
{
inline namespace v1
{

ProgressBar::ProgressBar(const Rect& rect,
                         int min, int max, int value) noexcept
    : ValueRangeWidget<int>(rect, min, max, value)
{
    set_boxtype(Theme::boxtype::rounded_borderfill);
}

void ProgressBar::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    draw_box(painter);

    auto width = detail::normalize<float>(value(), m_min, m_max, 0, w());

    theme().draw_rounded_gradient_box(painter,
                                      Rect(x(), y(), width, h()),
                                      palette().color(Palette::ColorId::highlight));

    string text = std::to_string(value()) + "%";
    m_dynamic_font = TextWidget::scale_font(box().size() * 0.75, text, m_dynamic_font);

    painter.set(this->palette().color(Palette::ColorId::text));
    painter.set(m_dynamic_font);

    auto text_size = painter.text_size(text);
    Rect target = detail::align_algorithm(text_size, box(), alignmask::center);

    painter.draw(target.point());
    painter.draw(text);
}

SpinProgress::SpinProgress(const Rect& rect, int min, int max, int value) noexcept
    : ValueRangeWidget<int>(rect, min, max, value)
{
}

void SpinProgress::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    auto dim = std::min(w(), h());
    float linew = dim / 10;
    float radius = dim / 2 - (linew / 2);
    float angle1 = detail::to_radians<float>(180, 0);
    float angle2 = detail::to_radians<float>(180, value() / 100. * 360.);

    painter.set(palette().color(Palette::ColorId::bg));
    painter.draw(box());
    painter.fill();
    painter.set_line_width(linew);

    painter.set(palette().color(Palette::ColorId::mid));
    painter.draw(Arc(center(), radius, 0.0f, 2 * M_PI));
    painter.stroke();

    painter.set(palette().color(Palette::ColorId::highlight));
    painter.draw(Arc(center(), radius, angle1, angle2));
    painter.stroke();

    string text = std::to_string(value());
    m_dynamic_font = TextWidget::scale_font(Size(dim, dim) * 0.75, text, m_dynamic_font);
    painter.set(this->palette().color(Palette::ColorId::text));
    painter.set(m_dynamic_font);

    auto text_size = painter.text_size(text);
    Rect target = detail::align_algorithm(text_size,
                                          this->box(),
                                          alignmask::center,
                                          0);
    painter.draw(target.point());
    painter.draw(text);
}

LevelMeter::LevelMeter(const Rect& rect,
                       int min, int max, int value) noexcept
    : ValueRangeWidget<int>(rect, min, max, value)
{
}

void LevelMeter::draw(Painter& painter, const Rect& rect)
{
    auto BAR_SPACING = 2;

    ignoreparam(rect);

    auto limit = egt::detail::normalize<float>(value(), m_min, m_max, m_num_bars, 0);
    auto barheight = h() / m_num_bars;

    for (uint32_t i = 0; i < m_num_bars; i++)
    {
        if (i >= limit)
            painter.set(palette().color(Palette::ColorId::highlight));
        else
            painter.set(palette().color(Palette::ColorId::dark));

        painter.draw(Rect(x(),  y() + i * barheight, w(), barheight - BAR_SPACING));
        painter.fill();
    }
}

AnalogMeter::AnalogMeter(const Rect& rect)
    : ValueRangeWidget<int>(rect, 0, 100, 0)
{
}

void AnalogMeter::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    auto cr = painter.context();

    painter.set_line_width(1.0);
    painter.set(m_font);

    cairo_text_extents_t textext;
    cairo_text_extents(cr.get(), "999", &textext);

    cairo_translate(cr.get(), x() + w() / 2, y() + h() - textext.height);

    float hw = w() / 2.0 - (textext.width * 2);

    // scale marks and labels
    for (double marks = 0.0; marks <= 100.0; marks += 10.0)
    {
        painter.set(palette().color(Palette::ColorId::highlight));

        cairo_move_to(cr.get(),
                      hw * std::cos(M_PI * marks * 0.01),
                      -hw * std::sin(M_PI * marks * 0.01));
        cairo_line_to(cr.get(),
                      (hw + 10.0) * std::cos(M_PI * marks * 0.01),
                      -(hw + 10.0) * std::sin(M_PI * marks * 0.01));

        cairo_stroke(cr.get());

        painter.set(palette().color(Palette::ColorId::text));

        char text[10];
        sprintf(text, "%2.0f", marks);

        cairo_text_extents(cr.get(), text, &textext);

        int width = textext.width;
        int height = textext.height;
        cairo_move_to(cr.get(),
                      (-(hw + 30.0) * std::cos(M_PI * marks * 0.01)) - ((double)width / 2.0),
                      (-(hw + 30.0) * std::sin(M_PI * marks * 0.01)) + ((double)height / 2.0));

        cairo_show_text(cr.get(), text);
        cairo_stroke(cr.get());
    }

    float value = this->value();
    painter.set(Palette::red);
    painter.set_line_width(1.5);

    cairo_move_to(cr.get(), 0.0, 0.0);
    cairo_line_to(cr.get(),
                  -hw * std::cos(M_PI * value * 0.01),
                  -hw * std::sin(M_PI * value * 0.01));
    cairo_stroke(cr.get());
}

}
}
