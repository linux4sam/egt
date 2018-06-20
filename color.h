/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

namespace mui
{

    class Color
    {
    public:

	// pre-defined colors
	static Color BLACK;
	static Color WHITE;
	static Color RED;
	static Color GREEN;
	static Color BLUE;
	static Color YELLOW;
	static Color CYAN;
	static Color MAGENTA;
	static Color SILVER;
	static Color GRAY;
	static Color LIGHTGRAY;
	static Color MAROON;
	static Color OLIVE;
	static Color PURPLE;
	static Color TEAL;
	static Color NAVY;
	static Color ORANGE;
	static Color AQUA;
	static Color LIGHTBLUE;

	Color(uint32_t c = 0)
	{
	    m_r = (c >> 24) & 0xFF;
	    m_g = (c >> 16) & 0xFF;
	    m_b = (c >> 8) & 0xFF;
	    m_a = c & 0xFF;
	}

	explicit Color(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255)
	    : m_r(r),
	      m_g(g),
	      m_b(b),
	      m_a(a)
	{}

	inline float redf() const { return m_r / 255.; }
	inline float greenf() const { return m_g / 255.; }
	inline float bluef() const { return m_b / 255.; }
	inline float alphaf() const { return m_a / 255.; }

	inline uint32_t red() const { return m_r; }
	inline uint32_t green() const { return m_g; }
	inline uint32_t blue() const { return m_b; }
	inline uint32_t alpha() const { return m_a; }
	inline void alpha(uint32_t a) { m_a = a; }

	// https://stackoverflow.com/questions/6615002/given-an-rgb-value-how-do-i-create-a-tint-or-shade

	Color shade(float factor) const
	{
	    return Color(redf() * (1. - factor),
			 greenf() * (1. - factor),
			 bluef() * (1. - factor),
			 alphaf());
	}

	Color tint(float factor) const
	{
	    return Color(redf() + (255. - redf()) * factor,
			 greenf() + (255. - greenf()) * factor,
			 bluef() + (255. - bluef()) * factor,
			 alphaf());
	}

    protected:
	uint32_t m_r;
	uint32_t m_g;
	uint32_t m_b;
	uint32_t m_a;
    };

    // TODO: remove and use palette
    extern Color FG_COLOR;
    extern Color BG_COLOR;
    extern Color TEXT_COLOR;
    extern Color GHIGH_COLOR;
    extern Color GLOW_COLOR;
    extern Color BORDER_COLOR;
}

#endif
