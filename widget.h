/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef WIDGET_H
#define WIDGET_H

#include "geometry.h"
#include "input.h"
#include "screen.h"
#include "utils.h"
#include <vector>
#include <string>
#include <cassert>
#include <cairo.h>
#include <map>

using namespace std;

class Widget
{
public:
	Widget(int x = 0, int y = 0, int w = 0, int h = 0);

	/**
	 * Draw the widget.
	 *
	 * Only the area under rect needs to be drawn.
	 */
	virtual void draw(const Rect& rect) = 0;

	/**
	 * Handle an event.
	 *
	 * Only the event ID is passed to this function. To get data associated
	 * with the event, you have to call other functions.
	 */
	virtual int handle(int event);

	/**
	 * Set the position of the widget.
	 */
	virtual void position(int x, int y);

	/**
	 * Set the size of the widget.
	 */
	virtual void size(int w, int h);

	/**
	 * Resize the widget.
	 * This will cause a redraw of the widget.
	 */
	virtual void resize(int w, int h);

	/**
	 * Move the widget.
	 * This will cause a redraw of the widget.
	 */
	virtual void move(int x, int y);

	const Rect& box() const { return m_box; }

	inline int w() const { return m_box.w; }
	inline int h() const { return m_box.h; }
	inline int x() const { return m_box.x; }
	inline int y() const { return m_box.y; }

	virtual ~Widget();

protected:
	Rect m_box;
};

class Window : public Widget
{
public:
	Window(int w, int h);

	void add(Widget* widget);

	/**
	 * Damage the rectangle of the entire widget.
	 */
	virtual	void damage();

	/**
	 * Damage the specified rectangle of the widget.
	 */
	virtual void damage(const Rect& rect);

#if 0
	Rect to_parent(const Rect& rect)
	{
		return Rect();
	}

	Rect to_child(const Rect& rect)
	{
		return Rect();
	}
#endif

	virtual int handle(int event);

	virtual void draw();

	virtual void draw(const Rect& rect);


protected:
	vector<Widget*> m_children;
	vector<Rect> m_damage;
};

class Image : public Widget
{
public:
	Image(const string& filename, int x = 0, int y = 0);

	virtual void draw(const Rect& rect);

	void scale(double scale);

	virtual ~Image();

protected:
	cairo_surface_t* m_image;
	cairo_surface_t* m_back;
	string m_filename;
	std::map<int,cairo_surface_t*> m_cache;
};

Window* main_window();

#endif
