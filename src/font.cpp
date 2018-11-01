/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/font.h"

namespace mui
{
    const std::string Font::FACE_ARIAL = "Arial Unicode";
    const std::string Font::FACE_TIMES = "Times Unicode";
    const std::string Font::FACE_COURIER = "Courier Unicode";

    // default font, which should be unicode
    static Font default_font = Font(Font::FACE_ARIAL, 16, Font::weightid::NORMAL,
                                    Font::slantid::NORMAL);

    void Font::set_default_font(const Font& font)
    {
        default_font = font;
    }

    Font::Font()
        : m_face(default_font.face()),
          m_size(default_font.size()),
          m_weight(default_font.weight()),
          m_slant(default_font.slant())
    {}

    Font::Font(const std::string& face, int size, weightid weight, slantid slant)
        : m_face(face),
          m_size(size),
          m_weight(weight),
          m_slant(slant)
    {}

    Font::Font(int size)
        : m_face(default_font.face()),
          m_size(size),
          m_weight(default_font.weight()),
          m_slant(default_font.slant())
    {}

    Font::Font(int size, weightid weight)
        : m_face(default_font.face()),
          m_size(size),
          m_weight(weight),
          m_slant(default_font.slant())
    {}

}
