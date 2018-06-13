/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef PIECHART_H
#define PIECHART_H

#include "widget.h"
#include <map>
#include <string>

namespace mui
{

class PieChart : public Widget
{
public:
	PieChart(const Point& point = Point(),
		 const Size& size = Size());

	void draw(const Rect& rect);

	void data(const std::map<std::string, float>& data)
	{
		damage();
		m_data = data;
	}

protected:
	std::map<std::string, float> m_data;
};

}

#endif
