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
#include <egt/serialize.h>
#include <egt/types.h>
#include <iosfwd>
#include <map>
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

    /// Scalar used for font size
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

    /// Default font face
    static constexpr const char* DEFAULT_FACE = "Free Sans";
    /// Default font weight
    static constexpr Font::Weight DEFAULT_WEIGHT = Font::Weight::normal;
    /// Default font size
    static constexpr Font::Size DEFAULT_SIZE = 18.f;
    /// Default font slant
    static constexpr Font::Slant DEFAULT_SLANT = Font::Slant::normal;

    constexpr Font() = default;

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
     * @param[in] weight The weight of the font.
     * @param[in] slant The slant of the font.
     */
    explicit Font(const std::string& face, Font::Size size,
                  Font::Weight weight = Weight::normal,
                  Font::Slant slant = Slant::normal);

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
    const std::string& face() const { return m_face; }

    /**
     * Set the face of the font.
     */
    void face(const std::string& face) { m_face = face; }

    /**
     * Get the size of the font.
     */
    Font::Size size() const { return m_size; }

    /**
     * Set the size of the font.
     */
    void size(const Font::Size& s) { m_size = s; }

    /**
     * Get the weight of the font.
     */
    Font::Weight weight() const { return m_weight; }

    /**
     * Set the weight of the font.
     */
    void weight(Font::Weight w) { m_weight = w; }

    /**
     * Get the slant of the font.
     */
    Font::Slant slant() const { return m_slant; }

    /**
     * Set the slant of the font.
     */
    void slant(Font::Slant s) { m_slant = s; }

    /**
     * Generates a FontConfig scaled font instance.
     *
     * Internally, this may use a font cache to limit regeneration of the same
     * font more than once.
     */
    cairo_scaled_font_t* scaled_font() const;

    /**
     * Serialize to the specified serializer.
     */
    void serialize(const std::string& name, Serializer& serializer) const;

    /**
     * Deserialize.
     */
    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs);

    /**
     * Clears any internal font cache.
     *
     * This will cause any new requests for fonts to be re-generated.
     */
    static void reset_font_cache();

    /**
     * Basically, this will clear the font cache and shutdown FontConfig which
     * will release all memory allocated by FontConfig.
     *
     * Typically, you should not need to call this.  However, this is useful for
     * debugging memory leaks and avoiding false positives in FontConfig.  Once
     * you call this function, requesting any Font to be created is likely to
     * not work out well.
     */
    static void shutdown_fonts();

protected:

    /// Font face name.
    std::string m_face{DEFAULT_FACE};

    /// Font size.
    Font::Size m_size{DEFAULT_SIZE};

    /// Font weight.
    Font::Weight m_weight{DEFAULT_WEIGHT};

    /// Font slant.
    Font::Slant m_slant{DEFAULT_SLANT};
};

/// @private
static_assert(detail::rule_of_5<Font>(), "must fulfill rule of 5");

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const Font& font);

/// Font operator
inline bool EGT_API operator==(const Font& lhs, const Font& rhs)
{
    return lhs.face() == rhs.face() &&
           detail::float_equal(lhs.size(), rhs.size()) &&
           lhs.weight() == rhs.weight() &&
           lhs.slant() == rhs.slant();
}

/// Font operator
inline bool EGT_API operator!=(const Font& lhs, const Font& rhs)
{
    return !(lhs == rhs);
}

}
}

#endif
