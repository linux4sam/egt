/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/font.h"

namespace egt
{
    const std::string Font::FACE_ARIAL = "Arial Unicode";
    const std::string Font::FACE_TIMES = "Times Unicode";
    const std::string Font::FACE_COURIER = "Courier Unicode";

    Font::Font()
        : m_face(global_font().face()),
          m_size(global_font().size()),
          m_weight(global_font().weight()),
          m_slant(global_font().slant())
    {}

    Font::Font(const std::string& face, int size, weightid weight, slantid slant)
        : m_face(face),
          m_size(size),
          m_weight(weight),
          m_slant(slant)
    {}

    Font::Font(int size)
        : m_face(global_font().face()),
          m_size(size),
          m_weight(global_font().weight()),
          m_slant(global_font().slant())
    {}

    Font::Font(int size, weightid weight)
        : m_face(global_font().face()),
          m_size(size),
          m_weight(weight),
          m_slant(global_font().slant())
    {}

    Font& global_font()
    {
        static Font* f = nullptr;

        if (!f)
        {
            f = new Font(Font::FACE_ARIAL, 16, Font::weightid::NORMAL,
                         Font::slantid::NORMAL);
        }

        return *f;
    }
}
