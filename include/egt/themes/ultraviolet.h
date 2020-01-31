/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_THEMES_ULTRAVIOLET_H
#define EGT_THEMES_ULTRAVIOLET_H

#include <egt/detail/meta.h>
#include <egt/theme.h>

namespace egt
{
inline namespace v1
{

/**
 * Ultra Violet Theme.
 */
class EGT_API UltraVioletTheme : public Theme
{
public:

    UltraVioletTheme() = default;

    virtual ~UltraVioletTheme() = default;

protected:

    virtual void init_palette() override
    {
        m_palette->clear();

        m_palette->set(Palette::ColorId::cursor, Palette::GroupId::normal, Palette::red);

        auto pattern = [](const Color & color)
        {
            Pattern pattern1;
            pattern1.step(0, color.tint(.20));
            pattern1.step(0.5, color);
            pattern1.step(1.0, color.tint(.20));
            return pattern1;
        };

        // colors light to dark
        std::vector<Color> colors =
        {
            Color::css("#f6f5f9"),
            Color::css("#e1ddea"),
            Color::css("#c3bcd6"),
            Color::css("#a59ac1"),
            Color::css("#8879ad"),
            Color::css("#6b5b95"),
            Color::css("#615286"),
            Color::css("#564977"),
            Color::css("#4b4068"),
            Color::css("#403759"),
            Color::css("#362e4a"),
        };

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::normal, colors[0]);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::normal, colors[5]);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::normal, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::normal, colors[7]);
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::normal, pattern(colors[7]));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::normal, colors[7]);
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::normal, Palette::white);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::normal, colors[0]);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::normal, colors[5]);

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::disabled, colors[0]);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::disabled, colors[2]);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::disabled, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::disabled, colors[2]);
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::disabled, pattern(colors[4]));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::disabled, colors[2]);
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::disabled, colors[2]);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::disabled, colors[0]);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::disabled, colors[2]);

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::active, colors[0]);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::active, colors[5]);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::active, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::active, colors[5]);
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::active, pattern(colors[10]));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::active, colors[7]);
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::active, colors[0]);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::active, colors[0]);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::active, colors[10]);

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::checked, colors[0]);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::checked, colors[5]);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::checked, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::checked, colors[7]);
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::checked, pattern(colors[7]));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::checked, colors[7]);
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::checked, Palette::white);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::checked, colors[10]);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::checked, colors[8]);
    }
};

}

}

#endif
