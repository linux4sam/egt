/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/geometry.h"
#include <iostream>

using namespace std;

namespace egt
{
    inline namespace v1
    {
        std::ostream& operator<<(std::ostream& os, const Point& point)
        {
            os << point.x << "," << point.y;
            return os;
        }

        std::ostream& operator<<(std::ostream& os, const Size& size)
        {
            os << size.w << "," << size.h;
            return os;
        }

        std::ostream& operator<< (std::ostream& os, const Rect& rect)
        {
            os << "[" << rect.x << "," << rect.y << " - " << rect.w << "," << rect.h << "]";
            return os;
        }

    }
}
