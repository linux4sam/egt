/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "painter.h"

namespace mui
{

    Painter::Painter()
    {}

    Painter::Painter(shared_cairo_t cr)
	: m_cr(cr)
    {
	begin();
    }

    Painter::~Painter()
    {
	end();
    }

    void Painter::begin()
    {
	cairo_save(m_cr.get());
    }

    void Painter::end()
    {
	cairo_restore(m_cr.get());
    }

    void Painter::set_color(const Color& color)
    {
	cairo_set_source_rgba(m_cr.get(),
			      color.redf(),
			      color.greenf(),
			      color.bluef(),
			      color.alphaf());
    }

    void Painter::draw_rectangle(const Rect& rect)
    {
	cairo_rectangle(m_cr.get(),
			rect.x,
			rect.y,
			rect.w,
			rect.h);
	cairo_stroke(m_cr.get());
    }

    void Painter::draw_fillrectangle(const Rect& rect)
    {
	cairo_rectangle(m_cr.get(),
			rect.x,
			rect.y,
			rect.w,
			rect.h);
	cairo_fill(m_cr.get());
    }

    void Painter::set_line_width(float width)
    {
	cairo_set_line_width(m_cr.get(), width);
    }

    void Painter::draw_line(const Point& start, const Point& end)
    {
	cairo_move_to(m_cr.get(), start.x, start.y);
	cairo_line_to(m_cr.get(), end.x, end.y);
    }

    void Painter::draw_line(const Point& start, const Point& end, float width)
    {
	set_line_width(width);
	cairo_move_to(m_cr.get(), start.x, start.y);
	cairo_line_to(m_cr.get(), end.x, end.y);
	cairo_stroke(m_cr.get());
    }

    void Painter::draw_image(const Point& point, shared_cairo_surface_t surface, bool bw)
    {
	double w = cairo_image_surface_get_width(surface.get());
	double h = cairo_image_surface_get_height(surface.get());
	cairo_set_source_surface(m_cr.get(), surface.get(), point.x, point.y);
	cairo_rectangle(m_cr.get(), point.x, point.y, w, h);
	cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_OVER);
	cairo_fill(m_cr.get());

	if (bw)
	{
	cairo_set_source_rgb(m_cr.get(), 0,0,0);
	cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_HSL_COLOR);
	cairo_mask_surface (m_cr.get(),
			    surface.get(),
			    point.x,
			    point.y);
	}
    }

    void Painter::draw_image(const Rect& rect, const Point& point, shared_cairo_surface_t surface)
    {
	cairo_set_source_surface(m_cr.get(), surface.get(), point.x, point.y);
	cairo_rectangle(m_cr.get(), rect.x, rect.y, rect.w, rect.h);
	cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_OVER);
	cairo_fill(m_cr.get());
    }

    void Painter::draw_arc(const Point& point, float radius, float angle1, float angle2)
    {
	cairo_arc(m_cr.get(), point.x, point.y, radius, angle1, angle2);
	cairo_stroke(m_cr.get());
    }

    void Painter::set_font(const Font& font)
    {
	cairo_select_font_face(m_cr.get(),
			       font.face().c_str(),
			       CAIRO_FONT_SLANT_NORMAL,
			       CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(m_cr.get(), font.size());
    }

    void Painter::draw_text(const Point& point, const std::string& str)
    {
	cairo_move_to(m_cr.get(), point.x, point.y);
	cairo_show_text(m_cr.get(), str.c_str());
	cairo_stroke(m_cr.get());
    }
}
