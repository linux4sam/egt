/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef CHART_H
#define CHART_H

#include <cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_KPLOT
#include "kplot.h"
#endif
#include "mui/widget.h"
#include <vector>
#include <map>

namespace mui
{

#ifdef HAVE_KPLOT
    class Chart : public Widget
    {
    public:
	Chart(const Point& point = Point(), const Size& size = Size());

	virtual void draw(const Rect& rect);

	virtual void data(std::vector<struct kpair> data) { m_data = data; }
    protected:

	std::vector<struct kpair> m_data;
    };
#endif

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
