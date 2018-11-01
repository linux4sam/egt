/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/object.h"
#include <iostream>

using namespace std;

namespace mui
{
    std::ostream& operator<<(std::ostream& os, const eventid& event)
    {
        os << (int) event;
        return os;
    }
}
