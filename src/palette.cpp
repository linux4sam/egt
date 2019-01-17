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
const Color& Palette::color(ColorId id, ColorGroup group) const
{
    assert(group < m_colors.size());
    assert(id < m_colors[group].size());

    return m_colors[group][id];
}

Palette& Palette::set(ColorId id, ColorGroup group, const Color& color)
{
    if (group >= m_colors.size())
        m_colors.resize(group + 1);

    if (id >= m_colors[group].size())
        m_colors[group].resize(id + 1);

    m_colors[group][id] = color;

    return *this;
}

void Palette::reset()
{
    set(Palette::BG, Palette::GROUP_NORMAL, Color::WHITE);
    set(Palette::FG, Palette::GROUP_NORMAL, Color::BLACK);
    set(Palette::LIGHT, Palette::GROUP_NORMAL, Color::WHITE);
    set(Palette::DARK, Palette::GROUP_NORMAL, Color::BLACK);
    set(Palette::MID, Palette::GROUP_NORMAL, Color(0xd1d2d4ff));
    set(Palette::TEXT, Palette::GROUP_NORMAL, Color::BLACK);
    set(Palette::TEXT_INVERT, Palette::GROUP_NORMAL, Color::WHITE);
    set(Palette::TEXTBG, Palette::GROUP_NORMAL, Color::WHITE);
    set(Palette::HIGHLIGHT, Palette::GROUP_NORMAL, Color(0xed2924ff));
    set(Palette::BORDER, Palette::GROUP_NORMAL, Color(0xed2924ff));

    set(Palette::BG, Palette::GROUP_DISABLED, Color(0xd1d2d4ff));
    set(Palette::FG, Palette::GROUP_DISABLED, Color::GRAY);
    set(Palette::LIGHT, Palette::GROUP_DISABLED, Color::GRAY);
    set(Palette::DARK, Palette::GROUP_DISABLED, Color::GRAY);
    set(Palette::MID, Palette::GROUP_DISABLED, Color::GRAY);
    set(Palette::TEXT, Palette::GROUP_DISABLED, Color(0x9b9b9dff));
    set(Palette::TEXT_INVERT, Palette::GROUP_DISABLED, Color(0x9b9b9dff));
    set(Palette::TEXTBG, Palette::GROUP_DISABLED, Color(0xd1d2d4ff));
    set(Palette::HIGHLIGHT, Palette::GROUP_DISABLED, Color(0xd1d2d4ff));
    set(Palette::BORDER, Palette::GROUP_DISABLED, Color(0x9c9d9dff));

    set(Palette::BG, Palette::GROUP_ACTIVE, Color(0xed2924ff));
    set(Palette::FG, Palette::GROUP_ACTIVE, Color::BLACK);
    set(Palette::LIGHT, Palette::GROUP_ACTIVE, Color::WHITE);
    set(Palette::DARK, Palette::GROUP_ACTIVE, Color::BLACK);
    set(Palette::MID, Palette::GROUP_ACTIVE, Color::GRAY);
    set(Palette::TEXT, Palette::GROUP_ACTIVE, Color::BLACK);
    set(Palette::TEXT_INVERT, Palette::GROUP_ACTIVE, Color::BLACK);
    set(Palette::TEXTBG, Palette::GROUP_ACTIVE, Color::WHITE);
    set(Palette::HIGHLIGHT, Palette::GROUP_ACTIVE, Color(0xe3edfaff));
    set(Palette::BORDER, Palette::GROUP_ACTIVE, Color(0xbfbfc0ff));
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
