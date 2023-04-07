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

constexpr HorizontalBitField AlignFlag::center_horizontal;
constexpr HorizontalBitField AlignFlag::left;
constexpr HorizontalBitField AlignFlag::right;

constexpr VerticalBitField AlignFlag::center_vertical;
constexpr VerticalBitField AlignFlag::top;
constexpr VerticalBitField AlignFlag::bottom;

constexpr HVBitField AlignFlag::center;

constexpr ExpandBitField AlignFlag::expand_horizontal;
constexpr ExpandBitField AlignFlag::expand_vertical;
constexpr ExpandBitField AlignFlag::expand;

template<>
const std::pair<HorizontalBitField, char const*> detail::EnumStrings<HorizontalBitField>::data[] =
{
    {AlignFlag::center_horizontal, "center_horizontal"},
    {AlignFlag::left, "left"},
    {AlignFlag::right, "right"},
};

template<>
const std::pair<VerticalBitField, char const*> detail::EnumStrings<VerticalBitField>::data[] =
{
    {AlignFlag::center_vertical, "center_vertical"},
    {AlignFlag::top, "top"},
    {AlignFlag::bottom, "bottom"},
};

template<>
const std::pair<ExpandBitField, char const*> detail::EnumStrings<ExpandBitField>::data[] =
{
    {AlignFlag::expand_horizontal, "expand_horizontal"},
    {AlignFlag::expand_vertical, "expand_vertical"},
    {AlignFlag::expand, "expand"},
};

template<>
const std::pair<HVBitField, char const*> detail::EnumStrings<HVBitField>::data[] =
{
    {AlignFlag::center, "center"},
};

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

EGT_API
std::list<AlignFlag> get(const BitFields<AlignFlag>& fields)
{
    std::list<AlignFlag> result;

    if (fields.is_set(AlignFlag::center))
        result.push_back(AlignFlag::center);
    else if (fields.is_set(AlignFlag::center_horizontal))
        result.push_back(AlignFlag::center_horizontal);
    else if (fields.is_set(AlignFlag::center_vertical))
        result.push_back(AlignFlag::center_vertical);

    if (fields.is_set(AlignFlag::left))
        result.push_back(AlignFlag::left);

    if (fields.is_set(AlignFlag::right))
        result.push_back(AlignFlag::right);

    if (fields.is_set(AlignFlag::top))
        result.push_back(AlignFlag::top);

    if (fields.is_set(AlignFlag::bottom))
        result.push_back(AlignFlag::bottom);

    if (fields.is_set(AlignFlag::expand))
        result.push_back(AlignFlag::expand);
    else if (fields.is_set(AlignFlag::expand_horizontal))
        result.push_back(AlignFlag::expand_horizontal);
    else if (fields.is_set(AlignFlag::expand_vertical))
        result.push_back(AlignFlag::expand_vertical);

    return result;
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
