/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_LIBPLANES

#include "event_loop.h"
#include "input.h"
#include "kmsscreen.h"
#include "planes/fb.h"
#include "planes/kms.h"
#include "planes/plane.h"
#include "widget.h"
#include "window.h"
#include <cairo.h>
#include <drm_fourcc.h>
#include <xf86drm.h>

using namespace std;

namespace mui
{

    PlaneWindow::PlaneWindow(const Size& size)
	: SimpleWindow(size.w, size.h)
    {
    }

    void PlaneWindow::damage() {}
    void PlaneWindow::damage(const Rect&) {}

    void PlaneWindow::position(int x, int y)
    {
	if (x != box().x || y != box().y)
	{
	    plane_set_pos(m_plane, x, y);
	}
    }

    void PlaneWindow::size(int w, int h)
    {
	if (w != box().w || h != box().h)
	{
	    assert(0);
	}
    }

    void PlaneWindow::draw()
    {
	plane_apply(m_plane);
    }

    PlaneWindow::~PlaneWindow()
    {
    }

    KMSScreen::KMSScreen()
    {
	int fd = drmOpen("atmel-hlcdc", NULL);
	assert(fd >= 0);

	struct kms_device *device = kms_device_open(fd);
	assert(device);

	//kms_device_dump(device);

	struct plane_data* plane = plane_create(device,
						DRM_PLANE_TYPE_PRIMARY,
						0,
						device->screens[0]->width,
						device->screens[0]->height,
						DRM_FORMAT_ARGB8888);

	assert(plane);
	plane_fb_map(plane);
	assert(plane->buf);

	cout << "dumb buffer " << plane_width(plane) << "," << plane_height(plane) << endl;

	plane_apply(plane);

	init(plane->buf, plane_width(plane), plane_height(plane));
    }

    void KMSScreen::flip(const vector<Rect>& damage)
    {
	IScreen::flip(damage);
    }

    KMSScreen::~KMSScreen()
    {
    }

}

#endif
