/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_THEMES_SHAMROCK_H
#define EGT_THEMES_SHAMROCK_H

#include <egt/theme.h>

namespace egt
{
inline namespace v1
{

/**
 * Shamrock Theme
 */
class EGT_API ShamrockTheme : public Theme
{
public:
    ShamrockTheme()
        : Theme("ShamrockTheme")
    {}

    ~ShamrockTheme() = default;

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
            Color::css("#eafffd"),
            Color::css("#bafff7"),
            Color::css("#74ffef"),
            Color::css("#2fffe7"),
            Color::css("#00e9cd"),
            Color::css("#00a591"),
            Color::css("#009382"),
            Color::css("#008373"),
            Color::css("#007265"),
            Color::css("#006256"),
            Color::css("#005248"),
        };

        m_palette.set(Palette::ColorId::bg, Palette::GroupId::normal, colors[0]);
        m_palette.set(Palette::ColorId::text, Palette::GroupId::normal, colors[5]);
        m_palette.set(Palette::ColorId::text_highlight, Palette::GroupId::normal, Palette::hotpink);
        m_palette.set(Palette::ColorId::border, Palette::GroupId::normal, colors[7]);
        m_palette.set(Palette::ColorId::button_bg, Palette::GroupId::normal, pattern(colors[7]));
        m_palette.set(Palette::ColorId::button_fg, Palette::GroupId::normal, colors[7]);
        m_palette.set(Palette::ColorId::button_text, Palette::GroupId::normal, colors[2]);
        m_palette.set(Palette::ColorId::label_bg, Palette::GroupId::normal, colors[0]);
        m_palette.set(Palette::ColorId::label_text, Palette::GroupId::normal, colors[5]);

        m_palette.set(Palette::ColorId::bg, Palette::GroupId::disabled, colors[0]);
        m_palette.set(Palette::ColorId::text, Palette::GroupId::disabled, colors[2]);
        m_palette.set(Palette::ColorId::text_highlight, Palette::GroupId::disabled, Palette::hotpink);
        m_palette.set(Palette::ColorId::border, Palette::GroupId::disabled, colors[2]);
        m_palette.set(Palette::ColorId::button_bg, Palette::GroupId::disabled, pattern(colors[4]));
        m_palette.set(Palette::ColorId::button_fg, Palette::GroupId::disabled, colors[2]);
        m_palette.set(Palette::ColorId::button_text, Palette::GroupId::disabled, colors[2]);
        m_palette.set(Palette::ColorId::label_bg, Palette::GroupId::disabled, colors[0]);
        m_palette.set(Palette::ColorId::label_text, Palette::GroupId::disabled, colors[2]);

        m_palette.set(Palette::ColorId::bg, Palette::GroupId::active, colors[0]);
        m_palette.set(Palette::ColorId::text, Palette::GroupId::active, colors[5]);
        m_palette.set(Palette::ColorId::text_highlight, Palette::GroupId::active, Palette::hotpink);
        m_palette.set(Palette::ColorId::border, Palette::GroupId::active, colors[5]);
        m_palette.set(Palette::ColorId::button_bg, Palette::GroupId::active, pattern(colors[10]));
        m_palette.set(Palette::ColorId::button_fg, Palette::GroupId::active, colors[7]);
        m_palette.set(Palette::ColorId::button_text, Palette::GroupId::active, colors[0]);
        m_palette.set(Palette::ColorId::label_bg, Palette::GroupId::active, colors[0]);
        m_palette.set(Palette::ColorId::label_text, Palette::GroupId::active, colors[10]);

        m_palette.set(Palette::ColorId::bg, Palette::GroupId::checked, colors[0]);
        m_palette.set(Palette::ColorId::text, Palette::GroupId::checked, colors[5]);
        m_palette.set(Palette::ColorId::text_highlight, Palette::GroupId::checked, Palette::hotpink);
        m_palette.set(Palette::ColorId::border, Palette::GroupId::checked, colors[7]);
        m_palette.set(Palette::ColorId::button_bg, Palette::GroupId::checked, pattern(colors[7]));
        m_palette.set(Palette::ColorId::button_fg, Palette::GroupId::checked, colors[7]);
        m_palette.set(Palette::ColorId::button_text, Palette::GroupId::checked, colors[2]);
        m_palette.set(Palette::ColorId::label_bg, Palette::GroupId::checked, colors[10]);
        m_palette.set(Palette::ColorId::label_text, Palette::GroupId::checked, colors[8]);
    }

    void init_draw() override;

    void init_font() override
    {
        m_font.face("Verdana");
    }
};

}

}

#endif
