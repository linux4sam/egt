/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>
#include "egt/svgdeserial.h"
#include "egt/text.h"
#include "detail/erawimage.h"
#include "detail/eraw.h"

namespace egt
{
inline namespace v1
{
namespace experimental
{

SVGDeserial::SVGDeserial(TopWindow& parent) noexcept
{
    parent.add(*this);
}

Surface SVGDeserial::DeSerialize(const unsigned char* buf, size_t len, std::shared_ptr<egt::Rect>& rect)
{
    return detail::ErawImage::load(buf, len, rect);
}

std::shared_ptr<GaugeLayer> SVGDeserial::AddWidgetByBuf(const unsigned char* buf, size_t len, bool show)
{
    auto rect = std::make_shared<Rect>();
    std::shared_ptr<GaugeLayer> widget;
    widget = std::make_shared<GaugeLayer>(Image(DeSerialize(buf, len, rect)));
    widget->box(*rect);
    if (show)
        widget->show();
    else
        widget->hide();
    add(widget);
    return widget;
}

std::shared_ptr<NeedleLayer> SVGDeserial::AddRotateWidgetByBuf(const unsigned char* buf, size_t len, int min, int max,
        int min_angle, int max_angle, bool clockwise, Point center)
{
    auto rect = std::make_shared<Rect>();
    std::shared_ptr<NeedleLayer> widget;
    widget = std::make_shared<NeedleLayer>(Image(DeSerialize(buf, len, rect)), min, max, min_angle, max_angle, clockwise);
    widget->box(*rect);
    widget->needle_center(center - widget->box().point());
    widget->needle_point(center);
    widget->show();
    add(widget);
    return widget;
}

std::shared_ptr<AnimationSequence> SVGDeserial::RotateAnimation(std::shared_ptr<NeedleLayer> widget,
        std::chrono::milliseconds duration, const EasingFunc& easein, const EasingFunc& easeout)
{
    auto animationup =
        std::make_shared<PropertyAnimator>(widget->min(), widget->max(), duration, easein);
    animationup->on_change([widget](PropertyAnimator::Value v)
    {
        widget->value(v);
    });

    auto animationdown =
        std::make_shared<PropertyAnimator>(widget->max(), widget->min(), duration, easeout);
    animationdown->on_change([widget](PropertyAnimator::Value v)
    {
        widget->value(v);
    });

    auto sequence = std::make_shared<AnimationSequence>(true);
    sequence->add(animationup);
    sequence->add(animationdown);
    sequence->start();
    return sequence;
}

void SVGDeserial::add_text_widget(const std::string& id, const std::string& txt, const egt::Rect& rect, egt::Font::Size size)
{
    auto text = std::make_shared<egt::TextBox>(txt, rect, egt::AlignFlag::center);
    text->border(0);
    text->font(egt::Font(size, egt::Font::Weight::normal));
    text->color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    text->color(egt::Palette::ColorId::text, egt::Palette::white);
    add(text);
    text->name(id);
}

void SVGDeserial::add_text_widget(const std::string& id, const std::string& txt, const egt::Rect& rect, egt::Font::Size size, egt::Color color)
{
    auto text = std::make_shared<egt::TextBox>(txt, rect);
    text->border(0);
    text->font(egt::Font(size, egt::Font::Weight::normal));
    text->color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    text->color(egt::Palette::ColorId::text, color);
    add(text);
    text->name(id);
}

bool SVGDeserial::is_point_in_line(egt::DefaultDim point, egt::DefaultDim start, egt::DefaultDim end)
{
    if (point >= start && point <= end)
        return true;
    else
        return false;
}

bool SVGDeserial::is_point_in_rect(egt::DisplayPoint& point, egt::Rect rect)
{
    if (point.x() >= rect.x()
        && point.x() <= (rect.x() + rect.width())
        && point.y() >= rect.y()
        && point.y() <= (rect.y() + rect.height()))
        return true;
    else
        return false;
}

}
}
}
