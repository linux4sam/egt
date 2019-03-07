/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/widget.h"
#include "egt/widgetflags.h"
#include <map>
#include <iostream>
#include <string>

namespace egt
{
inline namespace v1
{

std::ostream& operator<<(std::ostream& os, const Widget::flags_type& flags)
{
    static std::map<Widget::flag, std::string> strings;
    if (strings.empty())
    {
#define MAPITEM(p) strings[p] = #p
        MAPITEM(Widget::flag::plane_window);
        MAPITEM(Widget::flag::window);
        MAPITEM(Widget::flag::frame);
        MAPITEM(Widget::flag::disabled);
        MAPITEM(Widget::flag::readonly);
        MAPITEM(Widget::flag::active);
        MAPITEM(Widget::flag::invisible);
        MAPITEM(Widget::flag::grab_mouse);
        MAPITEM(Widget::flag::no_clip);
#undef MAPITEM
    }

    bool first = true;
    auto f = flags.get();
    for (auto& flag : f)
    {
        if (first)
            first = false;
        else
            os << " | ";
        os << strings[flag];
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Widget::flag& flag)
{
    static std::map<Widget::flag, std::string> strings;
    if (strings.empty())
    {
#define MAPITEM(p) strings[p] = #p
        MAPITEM(Widget::flag::plane_window);
        MAPITEM(Widget::flag::window);
        MAPITEM(Widget::flag::frame);
        MAPITEM(Widget::flag::disabled);
        MAPITEM(Widget::flag::readonly);
        MAPITEM(Widget::flag::active);
        MAPITEM(Widget::flag::invisible);
        MAPITEM(Widget::flag::grab_mouse);
#undef MAPITEM
    }

    os << strings[flag];
    return os;
}

}
}
