/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/enum.h"
#include "egt/detail/string.h"
#include "egt/widget.h"
#include "egt/widgetflags.h"
#include <iostream>
#include <map>
#include <string>

namespace egt
{
inline namespace v1
{

template<>
std::map<Widget::Flag, char const*> detail::EnumStrings<Widget::Flag>::data =
{
    {Widget::Flag::plane_window, "plane_window"},
    {Widget::Flag::window, "window"},
    {Widget::Flag::frame, "frame"},
    {Widget::Flag::disabled, "disabled"},
    {Widget::Flag::readonly, "readonly"},
    {Widget::Flag::active, "active"},
    {Widget::Flag::invisible, "invisible"},
    {Widget::Flag::grab_mouse, "grab_mouse"},
    {Widget::Flag::no_clip, "no_clip"},
    {Widget::Flag::no_layout, "no_layout"},
    {Widget::Flag::no_autoresize, "no_autoresize"},
    {Widget::Flag::checked, "checked"},
};

std::ostream& operator<<(std::ostream& os, const Widget::Flags& flags)
{
    return os << flags.to_string();
}

std::ostream& operator<<(std::ostream& os, const Widget::Flag& flag)
{
    return os << detail::enum_to_string(flag);
}

template<>
std::map<AlignFlag, char const*> detail::EnumStrings<AlignFlag>::data =
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
std::map<WindowHint, char const*> detail::EnumStrings<WindowHint>::data =
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
std::map<Orientation, char const*> detail::EnumStrings<Orientation>::data =
{
    {Orientation::horizontal, "horizontal"},
    {Orientation::vertical, "vertical"},
    {Orientation::flex, "flex"},
    {Orientation::none, "none"},
};

template<>
std::map<Justification, char const*> detail::EnumStrings<Justification>::data =
{
    {Justification::start, "start"},
    {Justification::middle, "middle"},
    {Justification::ending, "ending"},
    {Justification::justify, "justify"},
    {Justification::none, "none"},
};

}
}
