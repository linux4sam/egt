/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef X11SCREEN_H
#define X11SCREEN_H

#ifdef HAVE_X11

#include "screen.h"
#include <X11/Xlib.h>

namespace mui
{

    /**
     * Screen in an X11 window.
     */
    class X11Screen : public IScreen
    {
    public:
	X11Screen(const Size& size = Size(1024,1024), bool borderless = false);

	void flip(const std::vector<Rect>& damage);

	virtual ~X11Screen();

    protected:

	static void process(int fd, uint32_t mask, void *data);

	Display* m_display;
	Drawable m_window;
    };

}

#endif

#endif
