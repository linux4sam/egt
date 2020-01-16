/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_THEMES_MIDNIGHT_H
#define EGT_THEMES_MIDNIGHT_H

#include <egt/theme.h>

namespace egt
{
inline namespace v1
{

/**
 * Midnight Theme.
 */
class EGT_API MidnightTheme : public Theme
{
public:

    MidnightTheme() = default;

    virtual ~MidnightTheme() = default;

protected:

    virtual void init_palette() override
    {
        m_palette->clear();

        m_palette->set(Palette::ColorId::cursor, Palette::GroupId::normal, Palette::red);

        auto pattern = [](const Color & color)
        {
            Pattern pattern1;
            pattern1.step(0, color);
            pattern1.step(0.43, color.shade(.1));
            pattern1.step(0.5, color.shade(.15));
            pattern1.step(1.0, color.shade(.18));
            return pattern1;
        };

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::black);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::normal, Palette::white);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::normal, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::normal, Color(0xc0c5c1ff));
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::normal, pattern(Color(0xff9b29ff)));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::normal, Color(0xff9b29ff));
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::normal, Palette::black);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::normal, Palette::black);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::normal, Palette::white);

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::disabled, Palette::black);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::disabled, Palette::gray);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::disabled, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::disabled, Palette::gray);
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::disabled, pattern(Color(0xdcdcdcff)));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::disabled, Palette::gray);
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::disabled, Palette::gray);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::disabled, Palette::black);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::disabled, Color(Palette::gray));

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::active, Color(0xff9b29ff));
        m_palette->set(Palette::ColorId::text, Palette::GroupId::active, Palette::white);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::active, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::active, Color(0xc088c1ff));
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::active, pattern(Color(0xff8829ff)));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::active, Color(0xff9b29ff));
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::active, Palette::black);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::active, Color(0xff9b29ff));
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::active, Palette::white);

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::checked, Palette::black);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::checked, Palette::white);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::checked, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::checked, Color(0xc0c5c1ff));
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::checked, pattern(Color(0xff9b29ff)));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::checked, Color(0xff9b29ff));
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::checked, Palette::black);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::checked, Color(0xff9b29ff));
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::checked, Palette::white);
    }
};

}
}

#endif
