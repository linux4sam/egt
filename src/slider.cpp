/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/enum.h"
#include "egt/slider.h"

namespace egt
{
inline namespace v1
{

template<>
const std::map<Slider::SliderFlag, char const*> detail::EnumStrings<Slider::SliderFlag>::data
{
    {Slider::SliderFlag::rectangle_handle, "rectangle_handle"},
    {Slider::SliderFlag::square_handle, "square_handle"},
    {Slider::SliderFlag::round_handle, "round_handle"},
    {Slider::SliderFlag::show_labels, "show_labels"},
    {Slider::SliderFlag::show_label, "show_label"},
    {Slider::SliderFlag::origin_opposite, "origin_opposite"},
    {Slider::SliderFlag::consistent_line, "consistent_line"},
};

}
}
