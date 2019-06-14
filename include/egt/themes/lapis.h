/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_THEMES_LAPIS_H
#define EGT_THEMES_LAPIS_H

#include <egt/theme.h>

namespace egt
{
inline namespace v1
{

/**
 * Lapis Theme.
 */
class LapisTheme : public Theme
{
public:

    LapisTheme() = default;

    virtual ~LapisTheme() = default;

protected:

    virtual void init_palette() override
    {
        m_palette->set(Palette::ColorId::cursor, Palette::GroupId::normal, Palette::red);

        auto pattern = [](const Color & color)
        {
            Pattern pattern1;
            pattern1.step(0, color.tint(.20));
            pattern1.step(0.5, color);
            pattern1.step(1.0, color.tint(.20));
            return pattern1;
        };

        // colors dark to light
        std::vector<Color> colors =
        {
            Color::CSS("#002647"),
            Color::CSS("#002e56"),
            Color::CSS("#003564"),
            Color::CSS("#003d72"),
            Color::CSS("#004581"),
            Color::CSS("#004b8d"),
            Color::CSS("#0073d8"),
            Color::CSS("#2398ff"),
            Color::CSS("#6cbaff"),
            Color::CSS("#b6ddff"),
            Color::CSS("#e9f5ff"),
        };

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::normal, colors[0]);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::normal, colors[8]);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::normal, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::normal, colors[9]);
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::normal, pattern(colors[9]));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::normal, colors[9]);
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::normal, colors[7]);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::normal, colors[0]);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::normal, colors[8]);

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::disabled, colors[0]);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::disabled, colors[6]);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::disabled, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::disabled, colors[4]);
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::disabled, pattern(colors[4]));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::disabled, colors[4]);
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::disabled, colors[6]);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::disabled, colors[0]);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::disabled, colors[6]);

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::active, colors[0]);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::active, colors[7]);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::active, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::active, colors[7]);
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::active, pattern(colors[10]));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::active, colors[9]);
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::active, colors[0]);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::active, colors[10]);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::active, colors[0]);
    }
};

}

}

#endif
