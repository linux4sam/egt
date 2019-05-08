/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/alignment.h"
#include "egt/detail/math.h"
#include "egt/detail/string.h"
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
    set_boxtype(Theme::boxtype::blank);
    set_border(theme().default_border());
}

void ProgressBar::draw(Painter& painter, const Rect& rect)
{
    Drawer<ProgressBar>::draw(*this, painter, rect);
}

void ProgressBar::default_draw(ProgressBar& widget, Painter& painter, const Rect&)
{
    widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    auto b = widget.content_area();
    auto width = detail::normalize<float>(widget.value(), widget.min(), widget.max(), 0, b.w);

    if (width)
    {
        widget.theme().draw_box(painter,
                                Theme::boxtype::blank,
                                Rect(b.x, b.y, width, b.h),
                                Color(),
                                widget.color(Palette::ColorId::button_bg));
    }

    string text = std::to_string(widget.value()) + "%";
    /// @todo font
    auto dynamic_font = TextWidget::scale_font(b.size() * 0.75, text, Font());

    painter.set(widget.color(Palette::ColorId::label_text).color());
    painter.set(dynamic_font);
    auto size = painter.text_size(text);
    auto target = detail::align_algorithm(size, b, alignmask::center);
    painter.draw(target.point());
    painter.draw(text, true);
}

static const auto DEFAULT_PROGRESSBAR_SIZE = Size(200, 30);

Size ProgressBar::min_size_hint() const
{
    return DEFAULT_PROGRESSBAR_SIZE + Widget::min_size_hint();
}

SpinProgress::SpinProgress(const Rect& rect, int min, int max, int value) noexcept
    : ValueRangeWidget<int>(rect, min, max, value)
{
    set_boxtype(Theme::boxtype::blank);
}

void SpinProgress::draw(Painter& painter, const Rect& rect)
{
    Drawer<SpinProgress>::draw(*this, painter, rect);
}

void SpinProgress::default_draw(SpinProgress& widget, Painter& painter, const Rect&)
{
    widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    auto b = widget.content_area();

    auto dim = std::min(b.w, b.h);
    float linew = dim / 10;
    float radius = dim / 2 - (linew / 2);
    float angle1 = detail::to_radians<float>(180, 0);
    float angle2 = detail::to_radians<float>(180, widget.value() / 100. * 360.);

    painter.set_line_width(linew);
    painter.set(widget.color(Palette::ColorId::button_fg, Palette::GroupId::disabled).color());
    painter.draw(Arc(widget.center(), radius, 0.0f, 2 * detail::pi<float>()));
    painter.stroke();

    painter.set(widget.color(Palette::ColorId::button_fg).color());
    painter.draw(Arc(widget.center(), radius, angle1, angle2));
    painter.stroke();

    string text = std::to_string(widget.value());
    /// @todo font
    auto dynamic_font = TextWidget::scale_font(Size(dim, dim) * 0.75, text, Font());
    painter.set(dynamic_font);
    painter.set(widget.color(Palette::ColorId::text).color());
    auto size = painter.text_size(text);
    auto target = detail::align_algorithm(size, b, alignmask::center);
    painter.draw(target.point());
    painter.draw(text);
}

static const auto DEFAULT_SPINPROGRESS_SIZE = Size(100, 100);

Size SpinProgress::min_size_hint() const
{
    return DEFAULT_SPINPROGRESS_SIZE + Widget::min_size_hint();
}

LevelMeter::LevelMeter(const Rect& rect,
                       int min, int max, int value) noexcept
    : ValueRangeWidget<int>(rect, min, max, value)
{
    set_boxtype(Theme::boxtype::blank);
    set_padding(2);
}

void LevelMeter::draw(Painter& painter, const Rect& rect)
{
    Drawer<LevelMeter>::draw(*this, painter, rect);
}

void LevelMeter::default_draw(LevelMeter& widget, Painter& painter, const Rect&)
{
    widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    auto b = widget.content_area();

    auto limit = egt::detail::normalize<float>(widget.value(), widget.min(),
                 widget.max(), widget.num_bars(), 0);
    auto barheight = b.h / widget.num_bars();

    for (size_t i = 0; i < widget.num_bars(); i++)
    {
        auto color = widget.color(Palette::ColorId::button_fg, Palette::GroupId::disabled);
        if (i >= limit)
            color = widget.color(Palette::ColorId::button_fg);

        Rect rect(b.x,  b.y + i * barheight, b.w, barheight - widget.padding());

        widget.theme().draw_box(painter,
                                Theme::boxtype::blank,
                                rect,
                                widget.color(Palette::ColorId::border),
                                color);
    }
}

static const auto DEFAULT_LEVELMETER_SIZE = Size(40, 100);

Size LevelMeter::min_size_hint() const
{
    return DEFAULT_LEVELMETER_SIZE + Widget::min_size_hint();
}

AnalogMeter::AnalogMeter(const Rect& rect)
    : ValueRangeWidget<float>(rect, 0, 100, 0)
{
    set_boxtype(Theme::boxtype::blank);
}

void AnalogMeter::draw(Painter& painter, const Rect& rect)
{
    Drawer<AnalogMeter>::draw(*this, painter, rect);
}

void AnalogMeter::default_draw(AnalogMeter& widget, Painter& painter, const Rect&)
{
    /// @todo This needs to support setting font.

    static const auto tick_width = 1.0;

    widget.draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    auto b = widget.content_area();
    painter.set_line_width(tick_width);
    painter.set(Font());
    auto text_size = painter.text_size("999");

    auto cr = painter.context().get();
    cairo_translate(cr, b.center().x, b.y + b.h - text_size.h);

    auto dim = std::min(b.w / 2, b.h);
    float hw = dim - (text_size.w * 2.0);

    // ticks and labels
    for (float tick = 0.0; tick <= 100.0; tick += 10.0)
    {
        auto xangle = std::cos(detail::pi<float>() * tick * 0.01);
        auto yangle = std::sin(detail::pi<float>() * tick * 0.01);
        painter.set(widget.color(Palette::ColorId::button_fg, Palette::GroupId::disabled).color());
        painter.draw(Point(hw * xangle,
                           -hw * yangle),
                     Point((hw + 10.0) * xangle,
                           -(hw + 10.0) * yangle));
        painter.stroke();

        auto text = detail::format(tick, 0);
        painter.set(widget.color(Palette::ColorId::text).color());
        auto size = painter.text_size(text);
        painter.draw(Point(-(hw + 30.0) * xangle - size.w / 2.0,
                           -(hw + 30.0) * yangle - size.h / 2.0));
        painter.draw(text);
        painter.stroke();
    }

    // needle
    painter.set(widget.color(Palette::ColorId::button_fg).color());
    painter.set_line_width(tick_width * 2.0);
    painter.draw(Point(),
                 Point((-hw - 15.0) * std::cos(detail::pi<float>() * widget.value() * 0.01),
                       (-hw - 15.0) * std::sin(detail::pi<float>() * widget.value() * 0.01)));
    painter.stroke();
    painter.draw(Circle(Point(), 5));
    painter.fill();
}

static const auto DEFAULT_ANALOGMETER_SIZE = Size(200, 100);

Size AnalogMeter::min_size_hint() const
{
    return DEFAULT_ANALOGMETER_SIZE + Widget::min_size_hint();
}

}
}
