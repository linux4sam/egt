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
class ShamrockTheme : public Theme
{
public:

    ShamrockTheme() = default;

    virtual ~ShamrockTheme() = default;

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

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::palegreen);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::normal, Color(0x576060ff));
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::normal, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::normal, Palette::yellowgreen);
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::normal, Palette::seagreen);
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::normal, Palette::yellowgreen);
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::normal, Palette::white);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::normal, Palette::palegreen);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::normal, Color(0x576060ff));

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::disabled, Palette::palegreen);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::disabled, Palette::gray);
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::disabled, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::disabled, Color(0xc9c9caff));
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::disabled, pattern(0xdcdcdcff));
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::disabled, Color(0xc9c9caff));
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::disabled, Palette::gray);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::disabled, Palette::palegreen);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::disabled, Color(Palette::gray));

        m_palette->set(Palette::ColorId::bg, Palette::GroupId::active, Palette::yellowgreen);
        m_palette->set(Palette::ColorId::text, Palette::GroupId::active, Color(0x576060ff));
        m_palette->set(Palette::ColorId::text_highlight, Palette::GroupId::active, Palette::hotpink);
        m_palette->set(Palette::ColorId::border, Palette::GroupId::active, Palette::yellowgreen);
        m_palette->set(Palette::ColorId::button_bg, Palette::GroupId::active, Palette::teal);
        m_palette->set(Palette::ColorId::button_fg, Palette::GroupId::active, Palette::yellowgreen);
        m_palette->set(Palette::ColorId::button_text, Palette::GroupId::active, Palette::white);
        m_palette->set(Palette::ColorId::label_bg, Palette::GroupId::active, Palette::yellowgreen);
        m_palette->set(Palette::ColorId::label_text, Palette::GroupId::active, Palette::white);
    }

    virtual void init_draw() override;
};

}

}

#endif
