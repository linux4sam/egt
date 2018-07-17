/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef PAINTER_H
#define PAINTER_H

#include "font.h"
#include "palette.h"
#include "geometry.h"
#include <cairo.h>
#include <memory>

namespace mui
{

    using shared_cairo_surface_t =
	std::shared_ptr<cairo_surface_t>;

    using shared_cairo_t =
	std::shared_ptr<cairo_t>;

/**
 * <https://www.cairographics.org/manual/index.html>
 */
    class Painter
    {
    public:

	Painter();

	Painter(shared_cairo_t cr);

	virtual ~Painter();

	void begin();

	void end();

	void set_color(const Color& color);

	void draw_rectangle(const Rect& rect);

	void draw_fillrectangle(const Rect& rect);

	void set_line_width(float width);

	void draw_line(const Point& start, const Point& end);

	void draw_line(const Point& start, const Point& end, float width);

	void draw_image(const Point& point, shared_cairo_surface_t surface, bool bw = false);

	void draw_image(const Rect& rect, const Point& point, shared_cairo_surface_t surface);

	void draw_arc(const Point& point, float radius, float angle1, float angle2);

	/*
	  void stroke()
	  {
	  cairo_stroke(m_cr.get());
	  }

	  void paint()
	  {
	  cairo_paint(m_cr.get());
	  }

	  void fill()
	  {
	  cairo_fill(m_cr.get());
	  }
	*/

	void set_font(const Font& font);

	void draw_text(const Point& point, const std::string& str);

	shared_cairo_t context() const { return m_cr; }

    protected:

	shared_cairo_t m_cr;
    };

}

#endif
