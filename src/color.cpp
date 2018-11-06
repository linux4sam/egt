/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "egt/color.h"
#include <iostream>

using namespace std;

namespace egt
{

    const Color Color::TRANSPARENT = Color(0x00000000UL);
    const Color Color::BLACK = Color(0x000000ffUL);
    const Color Color::WHITE = Color(0xffffffffUL);
    const Color Color::RED = Color(0xff0000ffUL);
    const Color Color::GREEN = Color(0x00ff00ffUL);
    const Color Color::BLUE = Color(0x0000ffffUL);
    const Color Color::YELLOW = Color(0xffff00ffUL);
    const Color Color::CYAN = Color(0x00ffffffUL);
    const Color Color::MAGENTA = Color(0xff00ffffUL);
    const Color Color::SILVER = Color(0xc0c0c0ffUL);
    const Color Color::GRAY = Color(0x808080ffUL);
    const Color Color::LIGHTGRAY = Color(0xd3d3d3ffUL);
    const Color Color::MAROON = Color(0x800000ffUL);
    const Color Color::OLIVE = Color(0x808000ffUL);
    const Color Color::PURPLE = Color(0x800080ffUL);
    const Color Color::TEAL = Color(0x008080ffUL);
    const Color Color::NAVY = Color(0x000080ffUL);
    const Color Color::ORANGE = Color(0xffa500ffUL);
    const Color Color::AQUA = Color(0x00ffffffUL);
    const Color Color::LIGHTBLUE = Color(0xadd8e6ffUL);


    std::ostream& operator<<(std::ostream& os, const Color& color)
    {
        os << "(" << color.red() << "," << color.green() << "," << color.blue() << "." << color.alpha() << ")";
        return os;
    }

}
