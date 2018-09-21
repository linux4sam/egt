/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_FONT_H
#define MUI_FONT_H

/**
 * @file
 * @brief Working with fonts.
 */

#include <string>

namespace mui
{

    /**
     * Manages a single font.
     */
    class Font
    {
    public:

        constexpr static const char*  DEFAULT_FONT_FACE = "Arial Unicode";

        enum
        {
            WEIGHT_NORMAL,
            WEIGHT_BOLD
        };

        explicit Font(const std::string& face = DEFAULT_FONT_FACE,
                      int size = 16,
                      int weight = WEIGHT_BOLD);

        explicit Font(int size);

        const std::string& face() const { return m_face; }

        int size() const { return m_size; }

        int weight() const { return m_weight; }

    protected:

        std::string m_face;
        int m_size;
        int m_weight;
    };

}

#endif
