/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef KMSSCREEN_H
#define KMSSCREEN_H

#include "screen.h"
#include "window.h"
#include "geometry.h"

struct plane_data;

namespace mui
{

    /**
     * Window that resides in a hardware plane.
     *
     * This is a window that is composed by hardware. Doing operations like
     * changing position, alpha, scale in hardware results in a performance
     * speedup.
     */
    class PlaneWindow : public SimpleWindow
    {
    public:

	PlaneWindow(const Size& size = Size(100,100));

	virtual	void damage();

	virtual void damage(const Rect&);

	virtual void position(int x, int y);

	virtual void size(int w, int h);

	virtual void draw();

	virtual ~PlaneWindow();

    protected:
	struct plane_data* m_plane;
    };

    /**
     * Screen in an KMS dumb buffer.
     *
     * This uses libplanes to modeset and configure planes.
     */
    class KMSScreen : public IScreen
    {
    public:
	KMSScreen();

	void flip(const std::vector<Rect>& damage);

	virtual ~KMSScreen();

    protected:

    };

}

#endif
