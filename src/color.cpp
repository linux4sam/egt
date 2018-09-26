/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "color.h"
#include <iostream>

using namespace std;

namespace mui
{

    const Color Color::TRANSPARENT = 0x00000000UL;
    const Color Color::BLACK = 0x000000ffUL;
    const Color Color::WHITE = 0xffffffffUL;
    const Color Color::RED = 0xff0000ffUL;
    const Color Color::GREEN = 0x00ff00ffUL;
    const Color Color::BLUE = 0x0000ffffUL;
    const Color Color::YELLOW = 0xffff00ffUL;
    const Color Color::CYAN = 0x00ffffffUL;
    const Color Color::MAGENTA = 0xff00ffffUL;
    const Color Color::SILVER = 0xc0c0c0ffUL;
    const Color Color::GRAY = 0x808080ffUL;
    const Color Color::LIGHTGRAY = 0xd3d3d3ffUL;
    const Color Color::MAROON = 0x800000ffUL;
    const Color Color::OLIVE = 0x808000ffUL;
    const Color Color::PURPLE = 0x800080ffUL;
    const Color Color::TEAL = 0x008080ffUL;
    const Color Color::NAVY = 0x000080ffUL;
    const Color Color::ORANGE = 0xffa500ffUL;
    const Color Color::AQUA = 0x00ffffffUL;
    const Color Color::LIGHTBLUE = 0xadd8e6ffUL;

    std::ostream& operator<<(std::ostream& os, const Color& color)
    {
        os << "(" << color.red() << "," << color.green() << "," << color.blue() << "." << color.alpha() << ")";
        return os;
    }

}
