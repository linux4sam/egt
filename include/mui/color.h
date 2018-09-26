/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_COLOR_H
#define MUI_COLOR_H

/**
 * @file
 * @brief Working with colors.
 */

#include <cstdint>
#include <iosfwd>

namespace mui
{

    /**
     * RGBA color.
     */
    class Color
    {
    public:

        //@{
        /** @brief Pre-defined color. */
        static const Color TRANSPARENT;
        static const Color BLACK;
        static const Color WHITE;
        static const Color RED;
        static const Color GREEN;
        static const Color BLUE;
        static const Color YELLOW;
        static const Color CYAN;
        static const Color MAGENTA;
        static const Color SILVER;
        static const Color GRAY;
        static const Color LIGHTGRAY;
        static const Color MAROON;
        static const Color OLIVE;
        static const Color PURPLE;
        static const Color TEAL;
        static const Color NAVY;
        static const Color ORANGE;
        static const Color AQUA;
        static const Color LIGHTBLUE;
        //@}

        /**
         * Create a color with the specified RGBA value.
         *
        * @param[in] c RGBA value.
               */
        Color(uint32_t c = 0) noexcept
        {
            m_r = (c >> 24) & 0xFF;
            m_g = (c >> 16) & 0xFF;
            m_b = (c >> 8) & 0xFF;
            m_a = c & 0xFF;
        }

        /**
        * Create a color with the specified RGBA component values.
         *
        * @param[in] r Red component in range 0 - 255.
         * @param[in] g Green component in range 0 - 255.
         * @param[in] b Blue component in range 0 - 255.
         * @param[in] a Alpha component in range 0 - 255.
         */
        explicit Color(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255) noexcept
            : m_r(r),
              m_g(g),
              m_b(b),
              m_a(a)
        {}

        //@{
        /** @brief RGBA component values as a float from 0.0 to 1.0. */
        inline float redf() const { return m_r / 255.; }
        inline float greenf() const { return m_g / 255.; }
        inline float bluef() const { return m_b / 255.; }
        inline float alphaf() const { return m_a / 255.; }
        //@}

        //@{
        /** @brief RGBA component values as value from 0 to 255. */
        inline uint32_t red() const { return m_r; }
        inline uint32_t green() const { return m_g; }
        inline uint32_t blue() const { return m_b; }
        inline uint32_t alpha() const { return m_a; }
        //@}

        //@{
        /** @brief Set RGBA component values individually from 0 to 255. */
        inline void red(uint32_t r) { m_r = r; }
        inline void green(uint32_t g) { m_g = g; }
        inline void blue(uint32_t b) { m_b = b; }
        inline void alpha(uint32_t a) { m_a = a; }
        //@}

        /**
         * Create a shade (darker) color based off this color given a factor.
         *
         * The smaller the factor, the darker the shade.
         */
        inline Color shade(float factor) const
        {
            return Color(redf() * (1. - factor),
                         greenf() * (1. - factor),
                         bluef() * (1. - factor),
                         alphaf());
        }

        /**
         * Create a tint (lighter) color based off this color given a factor.
         *
         * The greater the factor the lighter the tint.
         */
        inline Color tint(float factor) const
        {
            return Color(redf() + (255. - redf()) * factor,
                         greenf() + (255. - greenf()) * factor,
                         bluef() + (255. - bluef()) * factor,
                         alphaf());
        }

        /**
         * Return a 32 bit ARGB pixel value for this color.
         */
        inline uint32_t pixel() const
        {
            return m_a << 24 | m_r << 16 | m_g << 8 | m_b;
        }

        /**
         * Return a 32 bit ARGB pre-multipled alpha pixel value for this color.
         */
        inline uint32_t prepixel() const
        {
            return (m_a << 24) |
                   (((m_r * m_a / 255) & 0xff) << 16) |
                   (((m_g * m_a / 255) & 0xff) << 8) |
                   ((m_b * m_a / 255) & 0xff);
        }

        Color& operator=(const Color&) = default;

        Color& operator=(uint32_t c)
        {
            m_r = (c >> 24) & 0xFF;
            m_g = (c >> 16) & 0xFF;
            m_b = (c >> 8) & 0xFF;
            m_a = c & 0xFF;

            return *this;
        }

    protected:
        uint32_t m_r;
        uint32_t m_g;
        uint32_t m_b;
        uint32_t m_a;
    };

    std::ostream& operator<<(std::ostream& os, const Color& color);
}

#endif
