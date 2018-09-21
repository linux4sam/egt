/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "font.h"

namespace mui
{

    Font::Font(const std::string& face, int size, int weight)
        : m_face(face),
          m_size(size),
          m_weight(weight)
    {
    }

    Font::Font(int size)
        : m_face(DEFAULT_FONT_FACE),
          m_size(size),
          m_weight(WEIGHT_BOLD)
    {
    }

}
