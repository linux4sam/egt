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

namespace detail
{
template<class T>
constexpr const T& clampf(const T& v, const T& lo = 0, const T& hi = 1)
{
    return assert(!(hi < lo)),
           (v < lo) ? lo : (hi < v) ? hi : v;
}
}

/**
 * 32 bit RGBA color.
 *
 * This manages the definition of a color, internally stored as separate
 * red, green, blue, and alpha components.
 *
 * @ref colors_colors
 */
class Color
{
public:

    using rgb_t = uint32_t;
    using rgba_t = uint32_t;
    using component_t = uint32_t;

    constexpr Color() = default;

    /**
     * Create a color with the specified RGBA value.
     *
     * @b Example
     * @code{.cpp}
     * auto a = Color(0xRRGGBBAA);
     * @endcode
     *
     * @param[in] c RGBA value.
     */
    explicit constexpr Color(rgba_t c) noexcept
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
     *
     * @param[in] color Pre-existing color.
     * @param[in] alpha Specific alpha value.
     */
    constexpr Color(const Color& color, component_t alpha) noexcept
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
    constexpr explicit Color(component_t r, component_t g, component_t b, component_t a = 255) noexcept
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

    //@{
    /** Get RGBA component value as a float from 0.0 to 1.0. */
    constexpr inline float redf() const { return m_rgba[0] / 255.f; }
    constexpr inline float greenf() const { return m_rgba[1] / 255.f; }
    constexpr inline float bluef() const { return m_rgba[2] / 255.f; }
    constexpr inline float alphaf() const { return m_rgba[3] / 255.f; }
    //@}

    //@{
    /** Set RGBA component value as a float from 0.0 to 1.0. */
    inline void redf(float v) { m_rgba[0] = detail::clampf(v) * 255; }
    inline void greenf(float v) { m_rgba[1] = detail::clampf(v) * 255; }
    inline void bluef(float v) { m_rgba[2] = detail::clampf(v) * 255; }
    inline void alphaf(float v) { m_rgba[3] = detail::clampf(v) * 255; }
    //@}

    //@{
    /** Get RGBA component value as value from 0 to 255. */
    constexpr inline component_t red() const { return assert(m_rgba[0] <= 255), m_rgba[0]; }
    constexpr inline component_t green() const { return assert(m_rgba[1] <= 255), m_rgba[1]; }
    constexpr inline component_t blue() const { return assert(m_rgba[2] <= 255), m_rgba[2]; }
    constexpr inline component_t alpha() const { return assert(m_rgba[3] <= 255), m_rgba[3]; }
    //@}

    //@{
    /** Set RGBA component value individually from 0 to 255. */
    inline void red(component_t r) { m_rgba[0] = r & 0xff; }
    inline void green(component_t g) { m_rgba[1] = g & 0xff; }
    inline void blue(component_t b) { m_rgba[2] = b & 0xff; }
    inline void alpha(component_t a) { m_rgba[3] = a & 0xff; }
    //@}

    /**
     * Get the hue, saturation, value, and alpha channel components of the color.
     *
     * @param[out] h Hue component in range 0 - 1.
     * @param[out] s Saturation component in range 0 - 1.
     * @param[out] v Value component in range 0 - 1.
     * @param[out] alpha Alpha component in range 0 - 1.
     */
    void get_hsvf(float* h, float* s, float* v, float* alpha = nullptr);

    /**
     * Get the hue, saturation, lightness, and alpha channel components of the color.
     *
     * @param[out] h Hue component in range 0 - 1.
     * @param[out] s Saturation component in range 0 - 1.
     * @param[out] l Lightness component in range 0 - 1.
     * @param[out] alpha Alpha component in range 0 - 1.
     */
    void get_hslf(float* h, float* s, float* l, float* alpha = nullptr);

    /**
     * Get the red, green, blue, and alpha channel components of the color.
     *
     * @param[out] r Red component in range 0 - 1.
     * @param[out] g Green component in range 0 - 1.
     * @param[out] b Blue component in range 0 - 1.
     * @param[out] alpha Alpha component in range 0 - 1.
     */
    void get_rgbaf(float* r, float* g, float* b, float* alpha = nullptr);

    /**
     * Create a color from only a RGB value with separate alpha channel component.
     *
     * @b Example
     * @code{.cpp}
     * auto a = Color::rgb(0xRRGGBB);
     * @endcode
     *
     * @param[in] c RGB value.
     * @param[in] alpha Alpha component in range 0 - 255.
     */
    constexpr static inline Color rgb(rgb_t c, component_t alpha = 255) noexcept
    {
        return Color(c << 8 | alpha);
    }

    /**
     * Create a color from float values.
     *
     * @param[in] r Component value as a float from 0.0 to 1.0.
     * @param[in] g Component value as a float from 0.0 to 1.0.
     * @param[in] b Component value as a float from 0.0 to 1.0.
     * @param[in] a Component value as a float from 0.0 to 1.0.
     */
    static inline Color rgbaf(float r, float g, float b, float a = 1.0)
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
     * @b Example
     * @code{.cpp}
     * auto a = Color::css("#0074D9");
     * @endcode
     */
    static inline Color css(const std::string& hex)
    {
        std::string str = hex;
        str.erase(std::remove(str.begin(), str.end(), '#'), str.end());
        return Color((std::stoi(str, nullptr, 16) << 8) | 0xff);
    }

    /**
     * Create a color from HSV values.
     *
     * @param[in] h Hue component in range 0 - 1.
     * @param[in] s Saturation component in range 0 - 1.
     * @param[in] v Value component in range 0 - 1.
     * @param[in] alpha Alpha component in range 0 - 1.
     */
    static Color hsvf(float h, float s, float v, float alpha = 1.0);

    /**
     * Create a color from HSL values.
     *
     * @param[in] h Hue component in range 0 - 1.
     * @param[in] s Saturation component in range 0 - 1.
     * @param[in] l Lightness component in range 0 - 1.
     * @param[in] alpha Alpha component in range 0 - 1.
     */
    static Color hslf(float h, float s, float l, float alpha = 1.0);

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
     * Create a new color by applying a hue value.
     */
    inline Color hue(float h) const
    {
        auto u = std::cos(h * detail::pi<float>() / 180.);
        auto w = std::sin(h * detail::pi<float>() / 180.);

        Color ret;
        ret.red((.299 + .701 * u + .168 * w) * red()
                + (.587 - .587 * u + .330 * w) * green()
                + (.114 - .114 * u - .497 * w) * blue());
        ret.green((.299 - .299 * u - .328 * w) * red()
                  + (.587 + .413 * u + .035 * w) * green()
                  + (.114 - .114 * u + .292 * w) * blue());
        ret.blue((.299 - .3 * u + 1.25 * w) * red()
                 + (.587 - .588 * u - 1.05 * w) * green()
                 + (.114 + .886 * u - .203 * w) * blue());
        return ret;
    }

    /**
     * Assign an RGBA value.
     *
     * @b Example
     * @code{.cpp}
     * Color a = 0xRRGGBBAA;
     * @endcode
     *
     * @param[in] c RGBA value.
     */
    Color& operator=(rgba_t c)
    {
        m_rgba[0] = (c >> 24) & 0xff;
        m_rgba[1] = (c >> 16) & 0xff;
        m_rgba[2] = (c >> 8) & 0xff;
        m_rgba[3] = c & 0xff;

        return *this;
    }

    template<class T>
    Color operator+(T scalar) const
    {
        return Color::rgbaf(redf() + scalar,
                            greenf() + scalar,
                            bluef() + scalar,
                            alphaf() + scalar);
    }

    template<class T>
    Color operator-(T scalar) const
    {
        return Color::rgbaf(redf() - scalar,
                            greenf() - scalar,
                            bluef() - scalar,
                            alphaf() - scalar);
    }

    template<class T>
    Color operator*(T scalar) const
    {
        return Color::rgbaf(redf() * scalar,
                            greenf() * scalar,
                            bluef() * scalar,
                            alphaf() * scalar);
    }

    template<class T>
    Color operator/(T scalar) const
    {
        return Color::rgbaf(redf() / scalar,
                            greenf() / scalar,
                            bluef() / scalar,
                            alphaf() / scalar);
    }

    Color operator+(const Color& rhs) const
    {
        return Color::rgbaf(redf() + rhs.redf(),
                            greenf() + rhs.greenf(),
                            bluef() + rhs.bluef(),
                            alphaf() + rhs.alphaf());
    }

    Color operator-(const Color& rhs) const
    {
        return Color::rgbaf(redf() - rhs.redf(),
                            greenf() - rhs.greenf(),
                            bluef() - rhs.bluef(),
                            alphaf() - rhs.alphaf());
    }

    Color operator*(const Color& rhs) const
    {
        return Color::rgbaf(redf() * rhs.redf(),
                            greenf() * rhs.greenf(),
                            bluef() * rhs.bluef(),
                            alphaf() * rhs.alphaf());
    }

    Color operator/(const Color& rhs) const
    {
        return Color::rgbaf(redf() / rhs.redf(),
                            greenf() / rhs.greenf(),
                            bluef() / rhs.bluef(),
                            alphaf() / rhs.alphaf());
    }

    /**
     * Perform linear interpolation between two colors in the HSV colorspace.
     *
     * @param a The first color.
     * @param b The second color.
     * @param t Value from 0 to 1.
     */
    static Color interp_hsv(const Color& a, const Color& b, float t);

    /**
     * Perform linear interpolation between two colors in the HSL colorspace.
     *
     * @param a The first color.
     * @param b The second color.
     * @param t Value from 0 to 1.
     */
    static Color interp_hsl(const Color& a, const Color& b, float t);

    /**
     * Perform linear interpolation between two colors in the RGB colorspace.
     *
     * @param a The first color.
     * @param b The second color.
     * @param t Value from 0 to 1.
     *
     * @note The interpolation happens in the RGB colorspace with the formula
     * c = a + (b - a) * t.
     */
    static Color interp_rgba(const Color& a, const Color& b, float t);

protected:

    /**
     * RGBA value.
     *
     * red[0], green[1], blue[2], alpha[3]
     *
     * @note std::valarray would have made more sense due to all the scalar
     * operations, but we lose constexpr with it.
     */
    std::array<component_t, 4> m_rgba{};
};

inline bool operator==(const Color& lhs, const Color& rhs)
{
    return lhs.red() == rhs.red() &&
           lhs.green() == rhs.green() &&
           lhs.blue() == rhs.blue() &&
           lhs.alpha() == rhs.alpha();
}

inline bool operator!=(const Color& lhs, const Color& rhs)
{
    return !(lhs == rhs);
}

template<class T>
inline Color operator+(T scalar, const Color& rhs)
{
    return Color::rgbaf(rhs.redf() + scalar,
                        rhs.greenf() + scalar,
                        rhs.bluef() + scalar,
                        rhs.alphaf() + scalar);
}

template<class T>
inline Color operator-(T scalar, const Color& rhs)
{
    return Color::rgbaf(rhs.redf() - scalar,
                        rhs.greenf() - scalar,
                        rhs.bluef() - scalar,
                        rhs.alphaf() - scalar);
}

template<class T>
inline Color operator*(T scalar, const Color& rhs)
{
    return Color::rgbaf(rhs.redf() * scalar,
                        rhs.greenf() * scalar,
                        rhs.bluef() * scalar,
                        rhs.alphaf() * scalar);
}

template<class T>
inline Color operator/(T scalar, const Color& rhs)
{
    return Color::rgbaf(rhs.redf() / scalar,
                        rhs.greenf() / scalar,
                        rhs.bluef() / scalar,
                        rhs.alphaf() / scalar);
}

std::ostream& operator<<(std::ostream& os, const Color& color);

/**
 * A Pattern which can store one or more colors at different offsets (steps)
 * which can be used to create gradients.
 */
class Pattern
{
public:
    Pattern() noexcept = default;

    // cppcheck-suppress noExplicitConstructor
    Pattern(const Color& color)
    {
        m_steps.insert(std::make_pair(0.f, color));
    }

    operator Color() const
    {
        if (!m_steps.empty())
            return m_steps.begin()->second;

        return Color();
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

namespace experimental
{

/**
 * A ColorMap contains a series of sequential color steps that can be used for
 * generating colors by interpolation.
 */
class ColorMap
{
public:

    enum class interpolation
    {
        rgba,
        hsv,
        hsl
    };

    using steps_array = std::vector<Color>;

    ColorMap() noexcept = default;

    explicit ColorMap(interpolation interp) noexcept
        : m_interp(interp)
    {}

    explicit ColorMap(const steps_array& steps, interpolation interp = interpolation::rgba)
        : m_interp(interp),
          m_steps(steps)
    {}

    /**
     * Append a color step.
     */
    ColorMap& step(const Color& color)
    {
        m_steps.emplace_back(color);
        return *this;
    }

    /**
     * Get a color at the specified offset.
     *
     * @param[in] t Offset from 0 to 1.
     */
    Color interp(float t) const;

    /**
     * Get a reference to the color steps array.
     */
    const steps_array& steps() const
    {
        return m_steps;
    }

    /**
     * Are there any color steps?
     */
    inline bool empty() const { return m_steps.empty(); }

    /**
     * Count the number of color steps.
     */
    inline size_t count() const { return m_steps.size(); }

protected:

    interpolation m_interp{interpolation::rgba};
    steps_array m_steps;
};

}

}
}

#endif
