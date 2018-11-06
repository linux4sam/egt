/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "egt/object.h"
#include <iostream>

using namespace std;

namespace egt
{
    std::ostream& operator<<(std::ostream& os, const eventid& event)
    {
        os << (int) event;
        return os;
    }
}
