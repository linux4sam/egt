/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/object.h"
#include <iostream>
#include <map>
#include <string>

using namespace std;

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
        MAPITEM(eventid::NONE);
        MAPITEM(eventid::MOUSE_DOWN);
        MAPITEM(eventid::MOUSE_UP);
        MAPITEM(eventid::MOUSE_MOVE);
        MAPITEM(eventid::BUTTON_DOWN);
        MAPITEM(eventid::BUTTON_UP);
        MAPITEM(eventid::MOUSE_DBLCLICK);
        MAPITEM(eventid::MOUSE_CLICK);
        MAPITEM(eventid::ENTER);
        MAPITEM(eventid::LEAVE);
        MAPITEM(eventid::KEYBOARD_DOWN);
        MAPITEM(eventid::KEYBOARD_UP);
        MAPITEM(eventid::KEYBOARD_REPEAT);
        MAPITEM(eventid::PROPERTY_CHANGED);
        MAPITEM(eventid::HIDE);
        MAPITEM(eventid::SHOW);
    }

    os << strings[event];
    return os;
}

}
}
