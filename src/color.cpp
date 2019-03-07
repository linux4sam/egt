/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/color.h"
#include <iostream>

using namespace std;

namespace egt
{
inline namespace v1
{

std::ostream& operator<<(std::ostream& os, const Color& color)
{
    os << "(" << color.red() << "," << color.green() << "," <<
       color.blue() << "," << color.alpha() << ")";
    return os;
}

}
}
