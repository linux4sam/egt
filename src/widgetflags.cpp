/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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

static const std::map<Widget::flag, std::string>& flag_strings()
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
        MAPITEM(Widget::flag::no_layout);
#undef MAPITEM
    }
    return strings;
}

std::ostream& operator<<(std::ostream& os, const Widget::flags_type& flags)
{
    if (!flags.empty())
    {
        // would be nice to use std::copy here, but no such luck
        detail::join(os, flags.get(), "|");
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Widget::flag& flag)
{
    const auto& strings = flag_strings();

    os << strings.at(flag);
    return os;
}

static const std::map<alignmask, std::string>& alignmask_strings()
{
    static std::map<alignmask, std::string> strings;
    if (strings.empty())
    {
#define MAPITEM(p) strings[p] = #p
        // MAPITEM(alignmask::none); not included intentionally
        MAPITEM(alignmask::left);
        MAPITEM(alignmask::right);
        MAPITEM(alignmask::center_horizontal);
        MAPITEM(alignmask::top);
        MAPITEM(alignmask::bottom);
        MAPITEM(alignmask::center_vertical);
        MAPITEM(alignmask::expand_horizontal);
        MAPITEM(alignmask::expand_vertical);
#undef MAPITEM
    }
    return strings;
}

std::ostream& operator<<(std::ostream& os, const alignmask& align)
{
    if (align == alignmask::none)
        return os;

    const auto& strings = alignmask_strings();

    bool first = true;
    for (auto& item : strings)
    {
        if ((item.first & align) == item.first)
        {
            if (first)
                first = false;
            else
                os << "|";

            os << item.second;
        }
    }

    return os;
}

}
}
