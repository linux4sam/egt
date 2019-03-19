/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/font.h"
#include <iostream>

namespace egt
{
inline namespace v1
{

const std::string Font::DEFAULT_FACE = "Arial Unicode";
const Font::weightid Font::DEFAULT_WEIGHT = Font::weightid::normal;
const int Font::DEFAULT_SIZE = 18;
const Font::slantid Font::DEFAULT_SLANT = Font::slantid::normal;

Font::Font()
    : m_face(DEFAULT_FACE),
      m_size(DEFAULT_SIZE),
      m_weight(DEFAULT_WEIGHT),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(const std::string& face, int size, weightid weight, slantid slant)
    : m_face(face),
      m_size(size),
      m_weight(weight),
      m_slant(slant)
{}

Font::Font(int size)
    : m_face(DEFAULT_FACE),
      m_size(size),
      m_weight(DEFAULT_WEIGHT),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(int size, weightid weight)
    : m_face(DEFAULT_FACE),
      m_size(size),
      m_weight(weight),
      m_slant(DEFAULT_SLANT)
{}

Font::Font(weightid weight)
    : m_face(DEFAULT_FACE),
      m_size(DEFAULT_SIZE),
      m_weight(weight),
      m_slant(DEFAULT_SLANT)
{}

std::ostream& operator<<(std::ostream& os, const Font& font)
{
    os << font.face() << ", " << font.size() << ", " <<
       static_cast<int>(font.weight()) << ", " << static_cast<int>(font.slant());
    return os;
}

}
}
