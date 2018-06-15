/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef FONT_H
#define FONT_H

#include <string>

namespace mui
{

    class Font
    {
    public:

	enum
	{
	    WEIGHT_NORMAL,
	    WEIGHT_BOLD
	};

	explicit Font(const std::string& face = "Arial", int size = 16, int weight = WEIGHT_BOLD);

	explicit Font(int size);

	const std::string& face() const { return m_face; }
	int size() const { return m_size; }
	int weight() const { return m_weight; }

    protected:

	std::string m_face;
	int m_size;
	int m_weight;
    };

}

#endif
