/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/enum.h"
#include "egt/detail/string.h"
#include "egt/widget.h"
#include "egt/widgetflags.h"
#include <ostream>
#include <string>

namespace egt
{
inline namespace v1
{

template<>
const std::pair<AlignFlag, char const*> detail::EnumStrings<AlignFlag>::data[] =
{
    {AlignFlag::center_horizontal, "center_horizontal"},
    {AlignFlag::center_vertical, "center_vertical"},
    {AlignFlag::center, "center"},
    {AlignFlag::left, "left"},
    {AlignFlag::right, "right"},
    {AlignFlag::top, "top"},
    {AlignFlag::bottom, "bottom"},
    {AlignFlag::expand_horizontal, "expand_horizontal"},
    {AlignFlag::expand_vertical, "expand_vertical"},
    {AlignFlag::expand, "expand"},
};

std::ostream& operator<<(std::ostream& os, const AlignFlags& align)
{
    return os << align.to_string();
}

template<>
const std::pair<WindowHint, char const*> detail::EnumStrings<WindowHint>::data[] =
{
    {WindowHint::automatic, "automatic"},
    {WindowHint::software, "software"},
    {WindowHint::overlay, "overlay"},
    {WindowHint::heo_overlay, "heo_overlay"},
    {WindowHint::cursor_overlay, "cursor_overlay"},
};

std::ostream& operator<<(std::ostream& os, const WindowHint& hint)
{
    return os << detail::enum_to_string(hint);
}

template<>
const std::pair<Orientation, char const*> detail::EnumStrings<Orientation>::data[] =
{
    {Orientation::horizontal, "horizontal"},
    {Orientation::vertical, "vertical"},
    {Orientation::flex, "flex"},
    {Orientation::none, "none"},
};

template<>
const std::pair<Justification, char const*> detail::EnumStrings<Justification>::data[] =
{
    {Justification::start, "start"},
    {Justification::middle, "middle"},
    {Justification::ending, "ending"},
    {Justification::justify, "justify"},
    {Justification::none, "none"},
};

}
}
