/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef WINDOW_H
#define WINDOW_H

#include "widget.h"
#include <vector>

namespace mui
{

    /**
     * @todo Rename this to Frame.
     */
    class SimpleWindow : public Widget
    {
    public:
	SimpleWindow(int w, int h);

	void add(Widget* widget);

	virtual void enter()
	{
	    active(true);
	    damage();
	}

	virtual void exit()
	{
	    active(false);
	}

	Size size() const { return Size(m_box.w,m_box.h); }

	// unsupported
	virtual void position(int x, int y) {}
	virtual void size(int w, int h) {}
	virtual void resize(int w, int h) {}

	virtual bool active() const { return m_active; }
	virtual void active(bool value);

	/**
	 * Damage the rectangle of the entire widget.
	 */
	virtual	void damage();

	/**
	 * Damage the specified rectangle of the widget.
	 */
	virtual void damage(const Rect& rect);

#if 0
	Rect to_screen(const Rect& rect)
	{
	    return Rect(x() + rect.x, y() + rect.y, rect.w, rect.h);
	}

	Rect to_child(const Rect& rect)
	{
	    return Rect(rect.x - x(), rect.y - y(), rect.w, rect.h);
	}
#endif

	virtual int handle(int event);

	virtual void draw();

	virtual void draw(const Rect& rect);

	void bgcolor(const Color& color) { m_bgcolor = color; }

    protected:

	virtual IScreen* screen() { assert(m_screen); return m_screen; }

	std::vector<Widget*> m_children;
	std::vector<Rect> m_damage;

	IScreen* m_screen;

	Color m_bgcolor;
    };

    class KMSOverlayScreen;

    /**
     * PlaneWindow seperates "changing attributes" and "applying attributes".
     * This maps to the libplanes plane_apply() function. Which, is the same way
     * event handle() vs. draw() works in this toolkit.
     */
    class PlaneWindow : public SimpleWindow
    {
    public:

	PlaneWindow(int w = 0, int h = 0);

	virtual void position(int x, int y);
	virtual void move(int x, int y);

	virtual void draw();

	// not supported
	virtual void size(int w, int h) {}
	virtual void resize(int w, int h) {}

	virtual ~PlaneWindow();
    };

    SimpleWindow*& main_window();
    std::vector<SimpleWindow*>& windows();
}

#endif
