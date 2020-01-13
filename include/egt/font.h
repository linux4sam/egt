/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FONT_H
#define EGT_FONT_H

/**
 * @file
 * @brief Working with fonts.
 */

#include <egt/detail/math.h>
#include <egt/types.h>
#include <iosfwd>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * Manages a font and properties of a font.
 *
 * A Font consists of a type face, a size, and a weight. The font face must
 * be installed on the system in order to use it.  Usually, if the specified
 * font face cannot be found on the system, a similar font face will be
 * selected.
 */
class EGT_API Font
{
public:

    using Size = float;

    /**
     * Font weights.
     */
    enum class Weight
    {
        normal = 0,
        bold = 1,
    };

    /**
     * Font slants.
     */
    enum class Slant
    {
        normal = 0,
        italic = 1,
        oblique = 2,
    };

    static const char* DEFAULT_FACE;
    static const Font::Weight DEFAULT_WEIGHT;
    static const Font::Size DEFAULT_SIZE;
    static const Font::Slant DEFAULT_SLANT;

    /**
     * Create a font based on the global default font.
     */
    Font();

    /**
     * Create a font based on the supplied parameters.
     *
     * @param[in] face The face name of the font.
     */
    explicit Font(const std::string& face);

    /**
     * Create a font based on the supplied parameters.
     *
     * @param[in] face The face name of the font.
     * @param[in] size The size of the font.
     */
    explicit Font(const std::string& face, Font::Size size);

    /**
     * Create a font based on the supplied parameters.
     *
     * @param[in] face The face name of the font.
     * @param[in] size The size of the font.
     * @param[in] weight The weight of the font.
     * @param[in] slant The slant of the font.
     */
    explicit Font(const std::string& face, Font::Size size, Font::Weight weight, Font::Slant slant);

    /**
     * Create a font based on the global default font, but with the
     * specified size.
     *
     * @param[in] size The size of the font.
     */
    explicit Font(Font::Size size);

    /**
     * Create a font based on the global default font, but with the
     * specified size and weight.
     *
     * @param[in] size The size of the font.
     * @param[in] weight The weight of the font.
     */
    explicit Font(Font::Size size, Font::Weight weight);

    /**
     * Create a font based on the global default font, but with the
     * specified weight.
     *
     * @param[in] weight The weight of the font.
     */
    explicit Font(Font::Weight weight);

    /**
     * Create a font based on the global default font, but with the
     * specified slant.
     *
     * @param[in] slant The slant of the font.
     */
    explicit Font(Font::Slant slant);

    /**
     * Get the face name the font.
     */
    inline const std::string& face() const { return m_face; }

    /**
     * Set the face of the font.
     */
    inline void face(const std::string& face) { m_face = face; }

    /**
     * Get the size of the font.
     */
    inline Font::Size size() const { return m_size; }

    /**
     * Set the size of the font.
     */
    inline void size(Font::Size s) { m_size = s; }

    /**
     * Get the weight of the font.
     */
    inline Font::Weight weight() const { return m_weight; }

    /**
     * Set the weight of the font.
     */
    inline void weight(Font::Weight w) { m_weight = w; }

    /**
     * Get the slant of the font.
     */
    inline Font::Slant slant() const { return m_slant; }

    /**
     * Set the slant of the font.
     */
    inline void slant(Font::Slant s) { m_slant = s; }

    /**
     * Generates a FontConfig scaled font instance.
     *
     * Internally, this may use a font cache to limit regeneration of the same
     * font more than once.
     */
    cairo_scaled_font_t* scaled_font() const;

protected:

    /**
     * Font face name.
     */
    std::string m_face;

    /**
     * Font size.
     */
    Font::Size m_size{};

    /**
     * Font weight.
     */
    Font::Weight m_weight;

    /**
     * Font slant.
     */
    Font::Slant m_slant;
};

EGT_API std::ostream& operator<<(std::ostream& os, const Font& font);

inline bool EGT_API operator==(const Font& lhs, const Font& rhs)
{
    return lhs.face() == rhs.face() &&
           detail::float_compare(lhs.size(), rhs.size()) &&
           lhs.weight() == rhs.weight() &&
           lhs.slant() == rhs.slant();
}

inline bool EGT_API operator!=(const Font& lhs, const Font& rhs)
{
    return !(lhs == rhs);
}

}
}

#endif
