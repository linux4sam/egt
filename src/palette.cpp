/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "palette.h"

namespace mui
{

    const Color& Palette::color(ColorId id, ColorGroup group) const
    {
        return m_colors[group][id];
    }

    void Palette::set(ColorId id, ColorGroup group, const Color& color)
    {
        if (group >= m_colors.size())
            m_colors.resize(group + 1);

        if (id >= m_colors[group].size())
            m_colors[group].resize(id + 1);

        m_colors[group][id] = color;
    }

    Palette& global_palette()
    {
        static Palette* p = nullptr;

        if (!p)
        {
            p = new Palette;
            p->set(Palette::BG, Palette::GROUP_NORMAL, Color::LIGHTGRAY);
            p->set(Palette::FG, Palette::GROUP_NORMAL, Color::BLACK);
            p->set(Palette::LIGHT, Palette::GROUP_NORMAL, Color::WHITE);
            p->set(Palette::DARK, Palette::GROUP_NORMAL, Color::BLACK);
            p->set(Palette::MID, Palette::GROUP_NORMAL, Color::GRAY);
            p->set(Palette::TEXT, Palette::GROUP_NORMAL, Color::BLACK);
            p->set(Palette::HIGHLIGHT, Palette::GROUP_NORMAL, Color::ORANGE);
            p->set(Palette::BORDER, Palette::GROUP_NORMAL, Color::GRAY);

            p->set(Palette::BG, Palette::GROUP_DISABLED, Color::GRAY);
            p->set(Palette::FG, Palette::GROUP_DISABLED, Color::GRAY);
            p->set(Palette::LIGHT, Palette::GROUP_DISABLED, Color::GRAY);
            p->set(Palette::DARK, Palette::GROUP_DISABLED, Color::GRAY);
            p->set(Palette::MID, Palette::GROUP_DISABLED, Color::GRAY);
            p->set(Palette::TEXT, Palette::GROUP_DISABLED, Color::GRAY);
            p->set(Palette::HIGHLIGHT, Palette::GROUP_DISABLED, Color::GRAY);
            p->set(Palette::BORDER, Palette::GROUP_DISABLED, Color::GRAY);

            p->set(Palette::BG, Palette::GROUP_ACTIVE, Color::GRAY);
            p->set(Palette::FG, Palette::GROUP_ACTIVE, Color::GRAY);
            p->set(Palette::LIGHT, Palette::GROUP_ACTIVE, Color::GRAY);
            p->set(Palette::DARK, Palette::GROUP_ACTIVE, Color::GRAY);
            p->set(Palette::MID, Palette::GROUP_ACTIVE, Color::GRAY);
            p->set(Palette::TEXT, Palette::GROUP_ACTIVE, Color::GRAY);
            p->set(Palette::HIGHLIGHT, Palette::GROUP_ACTIVE, Color::GRAY);
            p->set(Palette::BORDER, Palette::GROUP_ACTIVE, Color::GRAY);
        }

        return *p;
    }
}
