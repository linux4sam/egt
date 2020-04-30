/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/enum.h"
#include "egt/detail/string.h"
#include "egt/slider.h"

namespace egt
{
inline namespace v1
{

template<>
void SliderType<float>::draw_label(Painter& painter, float value)
{
    const auto b = this->content_area();
    auto handle = handle_box(value);

    if (m_orient == Orientation::horizontal)
        handle -= Point(0, b.height() / 2.);
    else
        handle -= Point(b.width() / 2., 0);

    const auto text = detail::format(value, 2);
    const auto f = TextWidget::scale_font(handle.size(), text, this->font());

    painter.set(this->color(Palette::ColorId::label_text));
    painter.set(f);

    const auto text_size = painter.text_size(text);
    const auto target = detail::align_algorithm(text_size,
                        handle,
                        AlignFlag::center,
                        5);
    painter.draw(target.point());
    painter.draw(text);
}

template<>
void SliderType<double>::draw_label(Painter& painter, double value)
{
    const auto b = this->content_area();
    auto handle = handle_box(value);

    if (m_orient == Orientation::horizontal)
        handle -= Point(0, b.height() / 2.);
    else
        handle -= Point(b.width() / 2., 0);

    const auto text = detail::format(value, 2);
    const auto f = TextWidget::scale_font(handle.size(), text, this->font());

    painter.set(this->color(Palette::ColorId::label_text));
    painter.set(f);

    const auto text_size = painter.text_size(text);
    const auto target = detail::align_algorithm(text_size,
                        handle,
                        AlignFlag::center,
                        5);
    painter.draw(target.point());
    painter.draw(text);
}

template<>
const std::pair<SliderBase::SliderFlag, char const*> detail::EnumStrings<SliderBase::SliderFlag>::data[] =
{
    {SliderBase::SliderFlag::rectangle_handle, "rectangle_handle"},
    {SliderBase::SliderFlag::square_handle, "square_handle"},
    {SliderBase::SliderFlag::round_handle, "round_handle"},
    {SliderBase::SliderFlag::show_labels, "show_labels"},
    {SliderBase::SliderFlag::show_label, "show_label"},
    {SliderBase::SliderFlag::origin_opposite, "origin_opposite"},
    {SliderBase::SliderFlag::consistent_line, "consistent_line"},
};

}
}
