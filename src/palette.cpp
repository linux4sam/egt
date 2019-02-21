/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/palette.h"
#include <cassert>

namespace egt
{
inline namespace v1
{

const Color Palette::transparent = Color(0x00000000UL);
const Color Palette::black = Color(0x000000ffUL);
const Color Palette::white = Color(0xffffffffUL);
const Color Palette::red = Color(0xff0000ffUL);
const Color Palette::green = Color(0x00ff00ffUL);
const Color Palette::blue = Color(0x0000ffffUL);
const Color Palette::yellow = Color(0xffff00ffUL);
const Color Palette::cyan = Color(0x00ffffffUL);
const Color Palette::magenta = Color(0xff00ffffUL);
const Color Palette::silver = Color(0xc0c0c0ffUL);
const Color Palette::gray = Color(0x808080ffUL);
const Color Palette::lightgray = Color(0xd3d3d3ffUL);
const Color Palette::maroon = Color(0x800000ffUL);
const Color Palette::olive = Color(0x808000ffUL);
const Color Palette::purple = Color(0x800080ffUL);
const Color Palette::teal = Color(0x008080ffUL);
const Color Palette::navy = Color(0x000080ffUL);
const Color Palette::orange = Color(0xffa500ffUL);
const Color Palette::aqua = Color(0x00ffffffUL);
const Color Palette::lightblue = Color(0xadd8e6ffUL);

const Color& Palette::color(ColorId id, GroupId group) const
{
    auto g = m_colors.find(group);
    if (g != m_colors.end())
    {
        auto c = g->second.find(id);
        if (c != g->second.end())
            return c->second;
    }

    static Color tmp;
    return tmp;
}

Palette& Palette::set(ColorId id, GroupId group, const Color& color)
{
    m_colors[group][id] = color;
    return *this;
}

void Palette::reset()
{
    set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::white);
    set(Palette::ColorId::fg, Palette::GroupId::normal, Palette::black);
    set(Palette::ColorId::light, Palette::GroupId::normal, Palette::white);
    set(Palette::ColorId::dark, Palette::GroupId::normal, Palette::black);
    set(Palette::ColorId::mid, Palette::GroupId::normal, Color(0xd1d2d4ff));
    set(Palette::ColorId::text, Palette::GroupId::normal, Palette::black);
    set(Palette::ColorId::text_invert, Palette::GroupId::normal, Palette::white);
    set(Palette::ColorId::textbg, Palette::GroupId::normal, Palette::white);
    set(Palette::ColorId::highlight, Palette::GroupId::normal, Color(0xed2924ff));
    set(Palette::ColorId::border, Palette::GroupId::normal, Color(0xed2924ff));

    set(Palette::ColorId::bg, Palette::GroupId::disabled, Color(0xd1d2d4ff));
    set(Palette::ColorId::fg, Palette::GroupId::disabled, Palette::gray);
    set(Palette::ColorId::light, Palette::GroupId::disabled, Palette::gray);
    set(Palette::ColorId::dark, Palette::GroupId::disabled, Palette::gray);
    set(Palette::ColorId::mid, Palette::GroupId::disabled, Palette::gray);
    set(Palette::ColorId::text, Palette::GroupId::disabled, Color(0x9b9b9dff));
    set(Palette::ColorId::text_invert, Palette::GroupId::disabled, Palette::black);
    set(Palette::ColorId::textbg, Palette::GroupId::disabled, Color(0xd1d2d4ff));
    set(Palette::ColorId::highlight, Palette::GroupId::disabled, Color(0xd1d2d4ff));
    set(Palette::ColorId::border, Palette::GroupId::disabled, Color(0x9c9d9dff));

    set(Palette::ColorId::bg, Palette::GroupId::active, Color(0xc1222bff));
    set(Palette::ColorId::fg, Palette::GroupId::active, Palette::black);
    set(Palette::ColorId::light, Palette::GroupId::active, Palette::white);
    set(Palette::ColorId::dark, Palette::GroupId::active, Palette::black);
    set(Palette::ColorId::mid, Palette::GroupId::active, Palette::gray);
    set(Palette::ColorId::text, Palette::GroupId::active, Palette::black);
    set(Palette::ColorId::text_invert, Palette::GroupId::active, Palette::white);
    set(Palette::ColorId::textbg, Palette::GroupId::active, Palette::white);
    set(Palette::ColorId::highlight, Palette::GroupId::active, Color(0xe3edfaff));
    set(Palette::ColorId::border, Palette::GroupId::active, Color(0xbfbfc0ff));
}

Palette& global_palette()
{
    static Palette* p = nullptr;

    if (!p)
    {
        p = new Palette;
        p->reset();
    }

    return *p;
}

}
}
