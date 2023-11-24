/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/progressbar.h"

namespace egt
{
inline namespace v1
{

template<>
const std::pair<ProgressBarStyle, char const*> detail::EnumStrings<ProgressBarStyle>::data[] =
{
    {ProgressBarStyle::left_to_right, "left_to_right"},
    {ProgressBarStyle::right_to_left, "right_to_left"},
    {ProgressBarStyle::top_to_bottom, "top_to_bottom"},
    {ProgressBarStyle::bottom_to_top, "bottom_to_top"},
};

}
}
