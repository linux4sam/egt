/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_COLOR_H
#define EGT_COLOR_H

/**
 * @file
 * @brief Working with colors.
 */

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <egt/detail/math.h>
#include <iosfwd>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * 32 bit RGBA color.
 *
 * This manages the definition of a color, internally stored as separate
 * red, green, blue, and alpha components.
 */
class Color
{
public:

    constexpr Color() = default;

    /**
     * Create a color with the specified RGBA value.
     *
     * @code{.cpp}
     * auto a = Color(0xRRGGBBAA);
     * @endcode
     *
     * @param[in] c RGBA value.
     */
    // cppcheck-suppress noExplicitConstructor
    constexpr Color(uint32_t c) noexcept
        : m_rgba
    {
        {
            (c >> 24) & 0xff,
            (c >> 16) & 0xff,
            (c >> 8) & 0xff,
            c & 0xff
        }
    }
    {}

    /**
     * Create a color from an existing color, but with the specified alpha value.
     */
    constexpr Color(const Color& color, uint32_t alpha) noexcept
        : m_rgba
    {
        {
            color.m_rgba[0],
                         color.m_rgba[1],
                         color.m_rgba[2],
                         alpha
        }
    }
    {}

    /**
     * Create a color with the specified RGBA component values.
     *
     * @param[in] r Red component in range 0 - 255.
     * @param[in] g Green component in range 0 - 255.
     * @param[in] b Blue component in range 0 - 255.
     * @param[in] a Alpha component in range 0 - 255.
     */
    constexpr explicit Color(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255) noexcept
        : m_rgba
    {
        {
            r & 0xff,
            g & 0xff,
            b & 0xff,
            a & 0xff
        }
    }
    {}

    /**
     * Create a color from only a RGB value with optional alpha.
     *
     * @code{.cpp}
     * auto a = Color::rgb(0xRRGGBB);
     * @endcode
     */
    constexpr static inline Color rgb(uint32_t c, uint32_t alpha = 0xff) noexcept
    {
        return {c << 8 | alpha};
    }

    /**
     * Create a color from float values.
     *
     * @param r Component value as a float from 0.0 to 1.0.
     * @param g Component value as a float from 0.0 to 1.0.
     * @param b Component value as a float from 0.0 to 1.0.
     * @param a Component value as a float from 0.0 to 1.0.
     */
    static inline Color from_float(float r, float g, float b, float a = 1.0)
    {
        Color result;
        result.redf(r);
        result.greenf(g);
        result.bluef(b);
        result.alphaf(a);
        return result;
    }

    /**
     * Create a Color with a hex CSS string.
     *
     * For example, the string #0074D9 can be used to specify a blue-like
     * color.
     *
     * @code{.cpp}
     * auto a = Color::CSS("#0074D9");
     * @endcode
     */
    static inline Color CSS(const std::string& hex)
    {
        std::string str = hex;
        str.erase(std::remove(str.begin(), str.end(), '#'), str.end());
        return Color((std::stoi(str, nullptr, 16) << 8) | 0xff);
    }

    /**
     * Apply a hue value to an existing Color, and return a new Color.
     */
    static inline Color hue(const Color& in, float h)
    {
        auto u = std::cos(h * detail::pi<float>() / 180.);
        auto w = std::sin(h * detail::pi<float>() / 180.);

        Color ret;
        ret.red((.299 + .701 * u + .168 * w)*in.red()
                + (.587 - .587 * u + .330 * w)*in.green()
                + (.114 - .114 * u - .497 * w)*in.blue());
        ret.green((.299 - .299 * u - .328 * w)*in.red()
                  + (.587 + .413 * u + .035 * w)*in.green()
                  + (.114 - .114 * u + .292 * w)*in.blue());
        ret.blue((.299 - .3 * u + 1.25 * w)*in.red()
                 + (.587 - .588 * u - 1.05 * w)*in.green()
                 + (.114 + .886 * u - .203 * w)*in.blue());
        return ret;
    }

    //@{
    /** RGBA component value as a float from 0.0 to 1.0. */
    inline float redf() const { return m_rgba[0] / 255.; }
    inline float greenf() const { return m_rgba[1] / 255.; }
    inline float bluef() const { return m_rgba[2] / 255.; }
    inline float alphaf() const { return m_rgba[3] / 255.; }
    //@}

    //@{
    /** Set RGBA component value as a float from 0.0 to 1.0. */
    inline void redf(float v) { m_rgba[0] = v * 255.; }
    inline void greenf(float v) { m_rgba[1] = v * 255.; }
    inline void bluef(float v) { m_rgba[2] = v * 255.; }
    inline void alphaf(float v) { m_rgba[3]  = v * 255.; }
    //@}

    //@{
    /** RGBA component value as value from 0 to 255. */
    inline uint32_t red() const { return m_rgba[0]; }
    inline uint32_t green() const { return m_rgba[1]; }
    inline uint32_t blue() const { return m_rgba[2]; }
    inline uint32_t alpha() const { return m_rgba[3]; }
    //@}

    //@{
    /** Set RGBA component value individually from 0 to 255. */
    inline void red(uint32_t r) { m_rgba[0] = r & 0xff; }
    inline void green(uint32_t g) { m_rgba[1] = g & 0xff; }
    inline void blue(uint32_t b) { m_rgba[2] = b & 0xff; }
    inline void alpha(uint32_t a) { m_rgba[3] = a & 0xff; }
    //@}

    /**
     * Create a shade (darker) color based off this color given a factor.
     *
     * The smaller the factor, the darker the shade.
     *
     * @param[in] factor Value from 0.0 to 1.0.
     */
    inline Color shade(float factor) const
    {
        return Color(red() * (1. - factor),
                     green() * (1. - factor),
                     blue() * (1. - factor),
                     alpha());
    }

    /**
     * Create a tint (lighter) color based off this color given a factor.
     *
     * The greater the factor the lighter the tint.
     *
     * @param[in] factor Value from 0.0 to 1.0.
     */
    inline Color tint(float factor) const
    {
        return Color(red() + ((255 - red()) * factor),
                     green() + ((255 - green()) * factor),
                     blue() + ((255 - blue()) * factor),
                     alpha());
    }

    /**
     * Return a 32 bit ARGB pixel value for this color.
     */
    inline uint32_t pixel_argb() const
    {
        return (m_rgba[3] & 0xff) << 24 |
               (m_rgba[0] & 0xff) << 16 |
               (m_rgba[1] & 0xff) << 8 |
               (m_rgba[2] & 0xff);
    }

    /**
     * Return a 32 bit ARGB pre-multiplied alpha pixel value for this color.
     */
    inline uint32_t prepixel_argb() const
    {
        return ((m_rgba[3] & 0xff) << 24) |
               (((m_rgba[0] * m_rgba[3] / 255) & 0xff) << 16) |
               (((m_rgba[1] * m_rgba[3] / 255) & 0xff) << 8) |
               ((m_rgba[2] * m_rgba[3] / 255) & 0xff);
    }

    Color& operator=(uint32_t c)
    {
        m_rgba[0] = (c >> 24) & 0xff;
        m_rgba[1] = (c >> 16) & 0xff;
        m_rgba[2] = (c >> 8) & 0xff;
        m_rgba[3] = c & 0xff;

        return *this;
    }

protected:

    /**
     * RGBA value.
     *
     * red[0], green[1], blue[2], alpha[3]
     */
    std::array<uint32_t, 4> m_rgba{};

    friend bool operator==(const Color& lhs, const Color& rhs);
};

inline bool operator==(const Color& lhs, const Color& rhs)
{
    return lhs.m_rgba[0] == rhs.m_rgba[0] &&
           lhs.m_rgba[1] == rhs.m_rgba[1] &&
           lhs.m_rgba[2] == rhs.m_rgba[2] &&
           lhs.m_rgba[3] == rhs.m_rgba[3];
}

inline bool operator!=(const Color& lhs, const Color& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Color& color);

/**
 * A Pattern which can store a single Color or color steps used for a gradient.
 */
struct Pattern
{
    Pattern() noexcept = default;

    // cppcheck-suppress noExplicitConstructor
    Pattern(const Color& color)
    {
        m_steps.insert(std::make_pair(0.f, color));
    }

    Color color() const
    {
        if (!m_steps.empty())
            return m_steps.begin()->second;

        return {};
    }

    Color& color()
    {
        if (!m_steps.empty())
            return m_steps.begin()->second;

        static Color tmp;
        return tmp;
    }

    explicit Pattern(const std::vector<std::pair<float, Color>>& steps)
    {
        for (auto& s : steps)
            m_steps.insert(s);
    }

    Pattern& step(float offset, const Color& color)
    {
        m_steps.insert(std::make_pair(offset, color));
        return *this;
    }

    const std::map<float, Color>& steps() const
    {
        return m_steps;
    }

protected:

    std::map<float, Color> m_steps;
};

}
}

#endif
