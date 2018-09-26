/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "font.h"

namespace mui
{
    const std::string Font::FACE_ARIAL = "Arial Unicode";
    const std::string Font::FACE_TIMES = "Times Unicode";
    const std::string Font::FACE_COURIER = "Courier Unicode";

    // default font, which should be unicode
    static Font default_font = Font(Font::FACE_ARIAL, 16, Font::WEIGHT_BOLD);

    void Font::set_default_font(const Font& font)
    {
        default_font = font;
    }

    Font::Font()
        : m_face(default_font.face()),
          m_size(default_font.size()),
          m_weight(default_font.weight())
    {}

    Font::Font(const std::string& face, int size, int weight)
        : m_face(face),
          m_size(size),
          m_weight(weight)
    {}

    Font::Font(int size)
        : m_face(default_font.face()),
          m_size(size),
          m_weight(default_font.weight())
    {}

    Font::Font(int size, int weight)
        : m_face(default_font.face()),
          m_size(size),
          m_weight(weight)
    {}

}
