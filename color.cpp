/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "color.h"

namespace mui
{

    Color Color::BLACK = 0x000000ffUL;
    Color Color::WHITE = 0xffffffffUL;
    Color Color::RED = 0xff0000ffUL;
    Color Color::GREEN = 0x00ff00ffUL;
    Color Color::BLUE = 0x0000ffffUL;
    Color Color::YELLOW = 0xffff00ffUL;
    Color Color::CYAN = 0x00ffffffUL;
    Color Color::MAGENTA = 0xff00ffffUL;
    Color Color::SILVER = 0xc0c0c0ffUL;
    Color Color::GRAY = 0x808080ffUL;
    Color Color::LIGHTGRAY = 0xd3d3d3ffUL;
    Color Color::MAROON = 0x800000ffUL;
    Color Color::OLIVE = 0x808000ffUL;
    Color Color::PURPLE = 0x800080ffUL;
    Color Color::TEAL = 0x008080ffUL;
    Color Color::NAVY = 0x000080ffUL;
    Color Color::ORANGE = 0xffa500ffUL;
    Color Color::AQUA = 0x00ffffffUL;
    Color Color::LIGHTBLUE = 0xadd8e6ffUL;

    // https://github.com/vurtun/nuklear/issues/349

    Color FG_COLOR = Color::OLIVE;
    Color BG_COLOR = Color::LIGHTGRAY;
    Color TEXT_COLOR = Color::BLACK;
    //Color HANDLE_COLOR = Color::ORANGE;
    Color GHIGH_COLOR = Color::WHITE;
    Color GLOW_COLOR = Color::WHITE;
    Color BORDER_COLOR = Color::GRAY;

}
