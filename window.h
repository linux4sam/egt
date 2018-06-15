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

    protected:

	virtual IScreen* screen() { assert(m_screen); return m_screen; }

	std::vector<Widget*> m_children;
	std::vector<Rect> m_damage;

	IScreen* m_screen;
    };

    class KMSOverlayScreen;

    class PlaneWindow : public SimpleWindow
    {
    public:

	PlaneWindow(KMSOverlayScreen* screen);

	virtual void position(int x, int y);

	virtual void draw();

	virtual ~PlaneWindow();

    protected:
    };

    SimpleWindow*& main_window();
    std::vector<SimpleWindow*>& windows();
}

#endif
