/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef WINDOW_H
#define WINDOW_H

#include "widget.h"
#include <vector>
#ifdef HAVE_LIBPLANES
#include <drm_fourcc.h>
#define DEFAULT_FORMAT DRM_FORMAT_ARGB8888
#else
#define DEFAULT_FORMAT 0
#endif

namespace mui
{

    /**
     * A window is a widget that is backed by a screen.
     *
     * The difference between a widget and a window is a window has zero or more
     * child widgets and manages the composition of drawing those widgets
     * together in the window.
     */
    class SimpleWindow : public Widget
    {
    public:
	SimpleWindow(const Size& size = Size(), uint32_t flags = FLAG_WINDOW_DEFAULT);

	void add(Widget* widget);
	void remove(Widget* widget);

	virtual void enter()
	{
	    show();
	    damage();
	}

	virtual void exit()
	{
	    hide();
	}

	Size size() const { return Size(m_box.w,m_box.h); }

	// unsupported
	//virtual void position(int x, int y) {}
	virtual void size(int w, int h) {}
	virtual void resize(int w, int h) {}

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

    protected:

	virtual IScreen* screen() { assert(m_screen); return m_screen; }

	std::vector<Widget*> m_children;
	std::vector<Rect> m_damage;

	IScreen* m_screen;
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

	explicit PlaneWindow(const Size& size = Size(),
			     uint32_t flags = FLAG_WINDOW_DEFAULT,
			     uint32_t format = DEFAULT_FORMAT);

	virtual void position(int x, int y);
	virtual void move(int x, int y);

	virtual void draw();

	// not supported
	virtual void size(int w, int h) {}

	virtual void resize(int w, int h);

	virtual ~PlaneWindow();

    protected:
	uint32_t m_format;
	bool m_dirty;
    };

    SimpleWindow*& main_window();
    std::vector<SimpleWindow*>& windows();
}

#endif
