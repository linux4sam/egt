/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/event.h"
#include "egt/input.h"
#include <iostream>
#include <map>
#include <string>

namespace egt
{
inline namespace v1
{

std::ostream& operator<<(std::ostream& os, const eventid& event)
{
    static std::map<eventid, std::string> strings;
    if (strings.empty())
    {
#define MAPITEM(p) strings[p] = #p
        MAPITEM(eventid::none);
        MAPITEM(eventid::raw_pointer_down);
        MAPITEM(eventid::raw_pointer_up);
        MAPITEM(eventid::raw_pointer_move);
        MAPITEM(eventid::pointer_click);
        MAPITEM(eventid::pointer_dblclick);
        MAPITEM(eventid::pointer_hold);
        MAPITEM(eventid::pointer_drag_start);
        MAPITEM(eventid::pointer_drag);
        MAPITEM(eventid::pointer_drag_stop);
        MAPITEM(eventid::enter);
        MAPITEM(eventid::leave);
        MAPITEM(eventid::keyboard_down);
        MAPITEM(eventid::keyboard_up);
        MAPITEM(eventid::keyboard_repeat);
        MAPITEM(eventid::property_changed);
        MAPITEM(eventid::input_property_changed);
        MAPITEM(eventid::hide);
        MAPITEM(eventid::show);
        MAPITEM(eventid::on_gain_focus);
        MAPITEM(eventid::on_lost_focus);
#undef MAPITEM
    }

    os << strings[event];
    return os;
}

namespace event
{

const Pointer& pointer()
{
    return Input::current().pointer();
}

const Keys& keys()
{
    return Input::current().keys();
}

}

}
}
