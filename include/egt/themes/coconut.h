/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_THEMES_COCONUT_H
#define EGT_THEMES_COCONUT_H

#include <egt/detail/meta.h>
#include <egt/theme.h>

namespace egt
{
inline namespace v1
{

/**
 * Coconut Theme.
 */
class EGT_API CoconutTheme : public Theme
{
protected:

    void init_palette() override
    {
        m_palette.clear();

        m_palette.set(Palette::ColorId::cursor, Palette::GroupId::normal, Palette::red);

        const auto pattern = [](const Color & color)
        {
            const Pattern patt(Pattern::Type::linear,
            {
                {0, color.tint(0.20)},
                {0.5, color},
                {1.0, color.tint(.20)},
            });
            return patt;
        };

        // colors light to dark
        std::vector<Color> colors =
        {
            Color::css("#fefefd"),
            Color::css("#fcfbfa"),
            Color::css("#f9f8f5"),
            Color::css("#f6f4ef"),
            Color::css("#f3f1ea"),
            Color::css("#f0ede5"),
            Color::css("#dfd9c7"),
            Color::css("#cec4aa"),
            Color::css("#bdb08c"),
            Color::css("#ac9b6e"),
            Color::css("#958456"),
        };

        m_palette.set(Palette::ColorId::bg, Palette::GroupId::normal, colors[0]);
        m_palette.set(Palette::ColorId::text, Palette::GroupId::normal, colors[8]);
        m_palette.set(Palette::ColorId::text_highlight, Palette::GroupId::normal, Palette::hotpink);
        m_palette.set(Palette::ColorId::border, Palette::GroupId::normal, colors[9]);
        m_palette.set(Palette::ColorId::button_bg, Palette::GroupId::normal, pattern(colors[9]));
        m_palette.set(Palette::ColorId::button_fg, Palette::GroupId::normal, colors[9]);
        m_palette.set(Palette::ColorId::button_text, Palette::GroupId::normal, colors[2]);
        m_palette.set(Palette::ColorId::label_bg, Palette::GroupId::normal, colors[0]);
        m_palette.set(Palette::ColorId::label_text, Palette::GroupId::normal, colors[8]);

        m_palette.set(Palette::ColorId::bg, Palette::GroupId::disabled, colors[0]);
        m_palette.set(Palette::ColorId::text, Palette::GroupId::disabled, colors[6]);
        m_palette.set(Palette::ColorId::text_highlight, Palette::GroupId::disabled, Palette::hotpink);
        m_palette.set(Palette::ColorId::border, Palette::GroupId::disabled, colors[4]);
        m_palette.set(Palette::ColorId::button_bg, Palette::GroupId::disabled, pattern(colors[4]));
        m_palette.set(Palette::ColorId::button_fg, Palette::GroupId::disabled, colors[4]);
        m_palette.set(Palette::ColorId::button_text, Palette::GroupId::disabled, colors[6]);
        m_palette.set(Palette::ColorId::label_bg, Palette::GroupId::disabled, colors[0]);
        m_palette.set(Palette::ColorId::label_text, Palette::GroupId::disabled, colors[6]);

        m_palette.set(Palette::ColorId::bg, Palette::GroupId::active, colors[0]);
        m_palette.set(Palette::ColorId::text, Palette::GroupId::active, colors[7]);
        m_palette.set(Palette::ColorId::text_highlight, Palette::GroupId::active, Palette::hotpink);
        m_palette.set(Palette::ColorId::border, Palette::GroupId::active, colors[7]);
        m_palette.set(Palette::ColorId::button_bg, Palette::GroupId::active, pattern(colors[10]));
        m_palette.set(Palette::ColorId::button_fg, Palette::GroupId::active, colors[9]);
        m_palette.set(Palette::ColorId::button_text, Palette::GroupId::active, colors[0]);
        m_palette.set(Palette::ColorId::label_bg, Palette::GroupId::active, colors[0]);
        m_palette.set(Palette::ColorId::label_text, Palette::GroupId::active, colors[10]);

        m_palette.set(Palette::ColorId::bg, Palette::GroupId::checked, colors[0]);
        m_palette.set(Palette::ColorId::text, Palette::GroupId::checked, colors[8]);
        m_palette.set(Palette::ColorId::text_highlight, Palette::GroupId::checked, Palette::hotpink);
        m_palette.set(Palette::ColorId::border, Palette::GroupId::checked, colors[9]);
        m_palette.set(Palette::ColorId::button_bg, Palette::GroupId::checked, pattern(colors[9]));
        m_palette.set(Palette::ColorId::button_fg, Palette::GroupId::checked, colors[9]);
        m_palette.set(Palette::ColorId::button_text, Palette::GroupId::checked, colors[2]);
        m_palette.set(Palette::ColorId::label_bg, Palette::GroupId::checked, colors[10]);
        m_palette.set(Palette::ColorId::label_text, Palette::GroupId::checked, colors[8]);
    }

    void init_font() override
    {
        m_font.face("Courier New");
    }
};

}

}

#endif
