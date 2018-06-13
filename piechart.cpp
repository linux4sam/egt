/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "piechart.h"
#include "color.h"

#include <cmath>
#include <vector>

using namespace std;

namespace mui
{

    PieChart::PieChart(const Point& point,
		       const Size& size)
	: Widget(point.x, point.y, size.w, size.h)
    {
    }

    static Color lightcolour()
    {
	static int index = 0;
	static vector<Color> colors;
	colors.push_back(Color::RED);
	colors.push_back(Color::GREEN);
	colors.push_back(Color::BLUE);
	colors.push_back(Color::YELLOW);
	colors.push_back(Color::CYAN);
	colors.push_back(Color::MAGENTA);
	colors.push_back(Color::SILVER);

	if (++index > colors.size())
	    index = 0;

	return colors[index];
    }

    void PieChart::draw(const Rect& rect)
    {
	shared_cairo_t cr = screen()->context();

	cairo_rectangle(cr.get(), x(), y(), w(), h());
	cairo_set_source_rgba(cr.get(),
			      BG_COLOR.redf(),
			      BG_COLOR.greenf(),
			      BG_COLOR.bluef(),
			      BG_COLOR.alphaf());
	cairo_fill(cr.get());

	float to_angle = M_PI * 3 / 2;
	float from_angle = to_angle;

	for (auto& i : m_data)
	{
	    to_angle += 2 * M_PI * i.second;
	    Color color = lightcolour();
	    cairo_set_source_rgba(cr.get(), color.redf(), color.greenf(), color.bluef(), color.alphaf());
	    cairo_move_to(cr.get(), x() + w()/2, y() + h()/2);
	    cairo_arc(cr.get(), x() + w()/2, y() + h()/2, w()*.45, from_angle, to_angle);
	    cairo_line_to(cr.get(), x() + w()/2, y() + h()/2);
	    cairo_fill(cr.get());
	    from_angle = to_angle;
	}

	cairo_set_font_size(cr.get(), w()/30);
	to_angle = M_PI * 3 / 2;
	from_angle = to_angle;
	for (auto& i : m_data)
	{
	    if (i.second < 0.01)
		continue;

	    to_angle += 2 * M_PI * i.second;
	    float label_angle = (from_angle + to_angle) / 2;
	    int label_x = w()/2 * (1.0 + 0.7 * cosf(label_angle));
	    int label_y = h()/2 * (1.0 + 0.7 * sinf(label_angle));
	    cairo_set_source_rgb(cr.get(), 0, 0, 0);
	    cairo_move_to(cr.get(), x() + label_x, y() + label_y);
	    cairo_show_text(cr.get(), i.first.c_str());
	    cairo_fill(cr.get());
	    from_angle = to_angle;
	}

    }

}
