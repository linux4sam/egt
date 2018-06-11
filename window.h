/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef WINDOW_H
#define WINDOW_H

#include "widget.h"

namespace mui
{

    class SimpleWindow : public Widget
    {
    public:
	SimpleWindow(int w, int h);

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
	std::vector<Widget*> m_children;
	std::vector<Rect> m_damage;
    };

    SimpleWindow* main_window();

}

#endif
