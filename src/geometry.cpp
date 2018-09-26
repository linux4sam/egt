/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "geometry.h"
#include <iostream>

using namespace std;

namespace mui
{

    Point operator-(Point lhs, const Point& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    Point operator+(Point lhs, const Point& rhs)
    {
        lhs += rhs;
        return lhs;
    }

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

    std::ostream& operator << (std::ostream& os, const Rect& rect)
    {
        os << "[" << rect.x << "," << rect.y << " - " << rect.w << "," << rect.h << "]";
        return os;
    }

}
