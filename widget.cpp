/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#include "widget.h"
#include "geometry.h"
#include "input.h"
#include "screen.h"
#include "utils.h"
#include <vector>
#include <string>
#include <cassert>
#include <cairo.h>
#include <iostream>

using namespace std;

static Window* the_window;

Window* main_window()
{
	return the_window;
}

Widget::Widget(int x, int y, int w, int h)
	: m_box(x, y, w, h)
{
}

int Widget::handle(int event)
{
	return 0;
}

void Widget::position(int x, int y)
{
	main_window()->damage(box());
	m_box.x = x;
	m_box.y = y;
}

void Widget::size(int w, int h)
{
	main_window()->damage(box());
	m_box.w = w;
	m_box.h = h;
}

void Widget::resize(int w, int h)
{
	if (w != box().w || h != box().h)
	{
		main_window()->damage(box());
		size(w, h);
		main_window()->damage(box());
	}
}

void Widget::move(int x, int y)
{
	if (x != box().x || y != box().y)
	{
		// damage old position
		main_window()->damage(box());

		position(x, y);

		// damage new position
		main_window()->damage(box());
	}
}

Widget::~Widget()
{}

Window::Window(int w, int h)
	: Widget(0, 0, w, h)
{
	the_window = this;
	damage();
}

void Window::add(Widget* widget)
{
	m_children.push_back(widget);
}

void Window::damage()
{
	damage(m_box);
}

/**
 * Mark the specified rect as a damaged area.
 *
 * This will merge the damaged area with any already existing damaged area that
 * it overlaps with into a super rectangle. It may be worth investigating if
 * this is optimal instead of removing the overlap and still having two
 * rectangles.  In some cases, this could result in significantly less
 * pixels.
 */
void Window::damage(const Rect& rect)
{
	// TODO: this should take into account widget coordinates
	if (!rect.is_clear())
	{
		bool found = false;
		for (auto& damage: m_damage)
		{
			if (Rect::is_intersect(damage,rect))
			{
				found = true;
				damage = Rect::merge(damage,rect);
			}
		}

		if (!found)
			m_damage.push_back(rect);
	}
}

int Window::handle(int event)
{
	for (auto& child: reverse_iterate(m_children))
	{
		switch (event)
		{
		case MOUSE_DOWN:
		case MOUSE_UP:
		case MOUSE_MOVE:
			if (Rect::point_inside(mouse_position(), child->box()))
				if (child->handle(event))
					return 1;

			break;
		}
	}

	return 0;
}

void Window::draw()
{
	bool found = false;

	for (auto& child: m_children)
	{
		for (auto& damage: m_damage)
		{
			if (Rect::is_intersect(damage,child->box()))
			{
				child->draw(damage);
				found = true;
			}
		}
	}

	if (found)
		screen()->flip(m_damage);

	m_damage.clear();
}

void Window::draw(const Rect& rect)
{
	draw();
}

Image::Image(const string& filename, int x, int y)
	: m_filename(filename)
{
	m_image = cairo_image_surface_create_from_png(filename.c_str());
	assert(m_image);

	m_cache.insert(std::pair<int,cairo_surface_t*>(1.0 * 100,m_image));

	position(x, y);
	size(cairo_image_surface_get_width(m_image),
	     cairo_image_surface_get_height(m_image));

	m_back = cairo_surface_create_similar(m_image,
					      CAIRO_CONTENT_COLOR_ALPHA,
					      cairo_image_surface_get_width(m_image),
					      cairo_image_surface_get_height(m_image));
	cairo_t *cr = cairo_create(m_back);
	cairo_set_source_surface(cr, m_image, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	cairo_destroy(cr);

	assert(m_back);

	main_window()->damage(box());
}

static cairo_surface_t *
scale_surface(cairo_surface_t *old_surface,
	      int old_width, int old_height,
	      int new_width, int new_height)
{
	cairo_surface_t *new_surface = cairo_surface_create_similar(old_surface,
								    CAIRO_CONTENT_COLOR_ALPHA,
								    new_width,
								    new_height);
	cairo_t *cr = cairo_create (new_surface);

	/* Scale *before* setting the source surface (1) */
	cairo_scale(cr, (double)new_width / old_width, (double)new_height / old_height);
	cairo_set_source_surface(cr, old_surface, 0, 0);

	/* To avoid getting the edge pixels blended with 0 alpha, which would
	 * occur with the default EXTEND_NONE. Use EXTEND_PAD for 1.2 or newer (2)
	 */
	cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REFLECT);

	/* Replace the destination with the source instead of overlaying */
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

	/* Do the actual drawing */
	cairo_paint(cr);

	cairo_destroy(cr);

	return new_surface;
}

void Image::scale(double scale)
{
	main_window()->damage(box());

	double neww = cairo_image_surface_get_width(m_back);
	double newh = cairo_image_surface_get_height(m_back);

	auto i = m_cache.find(scale * 100);
	if (i != m_cache.end())
	{
		m_image = i->second;
	}
	else
	{
		cout << "cache miss" << endl;

		m_image = scale_surface(m_back,
					neww, newh,
					neww * scale, newh * scale);
		m_cache.insert(std::pair<int,cairo_surface_t*>(scale * 100,m_image));
	}

	size(neww * scale, newh * scale);

	main_window()->damage(box());
}

void Image::draw(const Rect& rect)
{
	screen()->blit(m_image, rect.x, rect.y, rect.w, rect.h, box().x, box().y);
}

Image::~Image()
{
	for (auto& x: m_cache)
		cairo_surface_destroy(x.second);

	cairo_surface_destroy(m_back);
}
