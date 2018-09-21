/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_PALETTE_H
#define MUI_PALETTE_H

#include <mui/color.h>
#include <memory>
#include <vector>

namespace mui
{

    /**
     * The color palette defines a bank of colors used. Each widget is capable
     * of optionally maintaining its own palette if one is explicitly set,
     * otherwise it will default to a reference of the global palette.
     *
     * The palette can be extended by simply inserting more groups and collor
     * ids in the event widgets need to expand on the pre-defined colors.
     */
    class Palette
    {
    public:

        enum ColorGroup
        {
            GROUP_NORMAL = 0,
            GROUP_ACTIVE,
            GROUP_DISABLED
        };

        enum ColorId
        {
            BG = 0,
            FG,
            LIGHT,
            DARK,
            MID,
            TEXT,
            HIGHLIGHT,
            BORDER
        };

        const Color& color(ColorId id, ColorGroup group = GROUP_NORMAL) const;

        void set(ColorId id, ColorGroup group, const Color& color);

    protected:

        std::vector<std::vector<Color>> m_colors;
    };

    /**
     * Get a reference to the global palette.
     */
    Palette& global_palette();

}

#endif
