/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/enum.h"
#include "egt/radial.h"

namespace egt
{
inline namespace v1
{

template<>
const std::pair<experimental::Radial::RadialFlag, char const*> detail::EnumStrings<experimental::Radial::RadialFlag>::data[] =
{
    {experimental::Radial::RadialFlag::text_value, "text_value"},
    {experimental::Radial::RadialFlag::rounded_cap, "rounded_cap"},
    {experimental::Radial::RadialFlag::input_value, "input_value"},
};

}
}
