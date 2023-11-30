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
std::string SliderType<float>::format_label(float value)
{
    return detail::format(value, 2);
}

template<>
std::string SliderType<double>::format_label(double value)
{
    return detail::format(value, 2);
}

template<>
const std::pair<SliderBase::SliderFlag, char const*> detail::EnumStrings<SliderBase::SliderFlag>::data[] =
{
    {SliderBase::SliderFlag::rectangle_handle, "rectangle_handle"},
    {SliderBase::SliderFlag::square_handle, "square_handle"},
    {SliderBase::SliderFlag::round_handle, "round_handle"},
    {SliderBase::SliderFlag::show_labels, "show_labels"},
    {SliderBase::SliderFlag::show_label, "show_label"},
    {SliderBase::SliderFlag::inverted, "inverted"},
    {SliderBase::SliderFlag::consistent_line, "consistent_line"},
};

}
}
