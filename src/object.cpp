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
        MAPITEM(eventid::RAW_POINTER_DOWN);
        MAPITEM(eventid::RAW_POINTER_UP);
        MAPITEM(eventid::RAW_POINTER_MOVE);
        MAPITEM(eventid::POINTER_CLICK);
        MAPITEM(eventid::POINTER_DBLCLICK);
        MAPITEM(eventid::POINTER_HOLD);
        MAPITEM(eventid::POINTER_DRAG_START);
        MAPITEM(eventid::POINTER_DRAG);
        MAPITEM(eventid::POINTER_DRAG_STOP);
        MAPITEM(eventid::POINTER_BUTTON_DOWN);
        MAPITEM(eventid::POINTER_BUTTON_UP);
        MAPITEM(eventid::ENTER);
        MAPITEM(eventid::LEAVE);
        MAPITEM(eventid::KEYBOARD_DOWN);
        MAPITEM(eventid::KEYBOARD_UP);
        MAPITEM(eventid::KEYBOARD_REPEAT);
        MAPITEM(eventid::PROPERTY_CHANGED);
        MAPITEM(eventid::INPUT_PROPERTY_CHANGED);
        MAPITEM(eventid::HIDE);
        MAPITEM(eventid::SHOW);
#undef MAPITEM
    }

    os << strings[event];
    return os;
}

}
}
