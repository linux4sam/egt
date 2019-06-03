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
        MAPITEM(eventid::event1);
        MAPITEM(eventid::event2);
#undef MAPITEM
    }

    os << strings[event];
    return os;
}

void Event::grab(Widget* widget)
{
    stop();
    detail::mouse_grab(widget);
}

std::ostream& operator<<(std::ostream& os, const Pointer::button& btn)
{
    static std::map<Pointer::button, std::string> strings;
    if (strings.empty())
    {
#define MAPITEM(p) strings[p] = #p
        MAPITEM(Pointer::button::none);
        MAPITEM(Pointer::button::left);
        MAPITEM(Pointer::button::middle);
        MAPITEM(Pointer::button::right);
        MAPITEM(Pointer::button::touch);
#undef MAPITEM
    }

    os << strings[btn];
    return os;
}

std::ostream& operator<<(std::ostream& os, const Pointer& pointer)
{
    os << "point(" << pointer.point << ")" <<
       " drag_point(" << pointer.drag_start << ")" <<
       " btn(" << pointer.btn << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Key& key)
{
    os << "key(" << key.key << ")" <<
       " code(" << key.code << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Event& event)
{
    os << event.id();
    switch (event.id())
    {
    case eventid::raw_pointer_down:
    case eventid::raw_pointer_up:
    case eventid::raw_pointer_move:
    case eventid::pointer_click:
    case eventid::pointer_dblclick:
    case eventid::pointer_hold:
    case eventid::pointer_drag_start:
    case eventid::pointer_drag:
    case eventid::pointer_drag_stop:
        os << " " << event.pointer();
        break;
    case eventid::keyboard_down:
    case eventid::keyboard_up:
    case eventid::keyboard_repeat:
        os << " " << event.key();
        break;
    default:
        break;
    }
    return os;
}

}
}
