/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_PALETTE_H
#define EGT_PALETTE_H

/**
 * @file
 * @brief Color Palette definition.
 */

#include <egt/color.h>
#include <memory>
#include <map>

namespace egt
{
inline namespace v1
{

/**
 * @brief Color palette.
 *
 * The color Palette defines a bank of colors used by the Theme, and ultimately
 * widgets.
 *
 * The palette can be extended by simply inserting more groups and color
 * ids in the event the palette needs to be expanded to support more colors.
 */
class Palette
{
public:

    //@{
    /** @brief Pre-defined color. */
    static const Color transparent;
    static const Color black;
    static const Color white;
    static const Color red;
    static const Color green;
    static const Color blue;
    static const Color yellow;
    static const Color cyan;
    static const Color magenta;
    static const Color silver;
    static const Color gray;
    static const Color lightgray;
    static const Color maroon;
    static const Color olive;
    static const Color purple;
    static const Color teal;
    static const Color navy;
    static const Color orange;
    static const Color aqua;
    static const Color lightblue;
    //@}


    enum class GroupId
    {
        normal,
        active,
        disabled
    };

    enum class ColorId
    {
        bg,
        fg,
        light,
        dark,
        mid,
        text,
        text_invert,
        textbg,
        highlight,
        border
    };

    const Color& color(ColorId id, GroupId group = GroupId::normal) const;

    Palette& set(ColorId id, GroupId group, const Color& color);

    /**
     * Reset known colors back to a default state.
     */
    void reset();

protected:

    std::map<GroupId, std::map<ColorId, Color>> m_colors;
};

/**
 * Get a reference to the global palette.
 *
 * @return Palette reference.
 */
Palette& global_palette();
}
}

#endif
