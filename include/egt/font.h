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

#include <iosfwd>
#include <string>
#include <egt/types.h>

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
class Font
{
public:

    /**
     * Font weights.
     */
    enum class weightid
    {
        normal = 0,
        bold = 1,
    };

    /**
     * Font slants.
     */
    enum class slantid
    {
        normal = 0,
        italic = 1,
        oblique = 2,
    };

    static const char* DEFAULT_FACE;
    static const weightid DEFAULT_WEIGHT;
    static const int DEFAULT_SIZE;
    static const slantid DEFAULT_SLANT;

    /**
     * Create a font based on the global default font.
     */
    Font();

    /**
     * Create a font based on the supplied parameters.
     *
     * @param[in] face The face name of the font.
     * @param[in] size The size of the font.
     */
    explicit Font(const std::string& face, int size);

    /**
     * Create a font based on the supplied parameters.
     *
     * @param[in] face The face name of the font.
     * @param[in] size The size of the font.
     * @param[in] weight The weight of the font.
     * @param[in] slant The slant of the font.
     */
    explicit Font(const std::string& face, int size, weightid weight, slantid slant);

    /**
     * Create a font based on the global default font, but with the
     * specified size.
     *
     * @param[in] size The size of the font.
     */
    explicit Font(int size);

    /**
     * Create a font based on the global default font, but with the
     * specified size and weight.
     *
     * @param[in] size The size of the font.
     * @param[in] weight The weight of the font.
     */
    explicit Font(int size, weightid weight);

    explicit Font(weightid weight);

    explicit Font(slantid slant);

    /**
     * Get the face name the font.
     */
    const std::string& face() const { return m_face; }

    /**
     * Set the face of the font.
     */
    void set_face(const std::string& face) { m_face = face; }

    /**
     * Get the size of the font.
     */
    virtual int size() const { return m_size; }

    /**
     * Set the size of the font.
     */
    virtual void set_size(int s) { m_size = s; }

    /**
     * Get the weight of the font.
     */
    virtual weightid weight() const { return m_weight; }

    /**
     * Set the weight of the font.
     */
    virtual void set_weight(weightid w) { m_weight = w; }

    /**
     * Get the slant of the font.
     */
    virtual slantid slant() const { return m_slant; }

    virtual ~Font() = default;

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
    int m_size;

    /**
     * Font weight.
     */
    weightid m_weight;

    /**
     * Font slant.
     */
    slantid m_slant;
};

std::ostream& operator<<(std::ostream& os, const Font& font);

inline bool operator==(const Font& lhs, const Font& rhs)
{
    return lhs.face() == rhs.face() &&
           lhs.size() == rhs.size() &&
           lhs.weight() == rhs.weight() &&
           lhs.slant() == rhs.slant();
}

inline bool operator!=(const Font& lhs, const Font& rhs)
{
    return !(lhs == rhs);
}

}
}

#endif
