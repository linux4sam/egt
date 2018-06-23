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

    KMSOverlayScreen::KMSOverlayScreen(struct plane_data* plane)
	: m_plane(plane)
    {
	init(plane->buf, plane_width(plane), plane_height(plane));
    }

    void KMSOverlayScreen::position(int x, int y)
    {
	plane_set_pos(m_plane, x, y);
    }

    int KMSOverlayScreen::gem()
    {
	return m_plane->gem_name;
    }

    void KMSOverlayScreen::apply()
    {
	plane_apply(m_plane);
    }

    KMSOverlayScreen::~KMSOverlayScreen()
    {
    }

    static KMSScreen* the_kms = 0;

    KMSScreen::KMSScreen()
    {
	m_fd = drmOpen("atmel-hlcdc", NULL);
	assert(m_fd >= 0);

	m_device = kms_device_open(m_fd);
	assert(m_device);

	//kms_device_dump(device);

	struct plane_data* plane = plane_create(m_device,
						DRM_PLANE_TYPE_PRIMARY,
						0,
						m_device->screens[0]->width,
						m_device->screens[0]->height,
						DRM_FORMAT_ARGB8888);

	assert(plane);
	plane_fb_map(plane);
	assert(plane->buf);

	cout << "dumb buffer " << plane_width(plane) << "," <<
	    plane_height(plane) << endl;

	plane_apply(plane);

	init(plane->buf, plane_width(plane), plane_height(plane));

	the_kms = this;
    }

    KMSScreen* KMSScreen::instance()
    {
	return the_kms;
    }

    /*
    static uint32_t fb_to_primefd(struct kms_framebuffer* fb)
    {
	int prime_fd;
	int r = drmPrimeHandleToFD(fb->device->fd, fb->handle, DRM_CLOEXEC, &prime_fd);
	if (r < 0)
	    printf("cannot get prime-fd for handle\n");
	else
	    printf("fb 0x%x: created prime fd %d\n", fb->id, prime_fd);

	return prime_fd;
    }

*/

    struct plane_data* KMSScreen::allocate_overlay(const Size& size, uint32_t format)
    {
	// TODO
	static int index = 0;
	struct plane_data* plane = plane_create(m_device,
						DRM_PLANE_TYPE_OVERLAY,
						index++,
						size.w,
						size.h,
						format);

	assert(plane);
	plane_fb_map(plane);
	assert(plane->buf);

	plane_set_pos(plane, 0, 0);

	cout << "overlay dumb buffer " << plane_width(plane) << "," <<
	    plane_height(plane) << endl;

	plane_apply(plane);

	return plane;
    }

    uint32_t KMSScreen::count_planes(int type)
    {
	uint32_t total = 0;
	for (uint32_t x = 0; x < m_device->num_planes;x++)
	{
	    if ((int)m_device->planes[x]->type == type)
		total++;
	}
	return total;
    }

    void KMSScreen::flip(const vector<Rect>& damage)
    {
	IScreen::flip(damage);
    }

    KMSScreen::~KMSScreen()
    {
	kms_device_close(m_device);
	drmClose(m_fd);
    }

}
#endif
