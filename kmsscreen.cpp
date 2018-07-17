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

    void* KMSOverlayScreen::raw()
    {
	return m_plane->buf;
    }

    void KMSOverlayScreen::position(int x, int y)
    {
	plane_set_pos(m_plane, x, y);
    }

    void KMSOverlayScreen::scale(float scale)
    {
	plane_set_scale(m_plane, scale);
    }

    float KMSOverlayScreen::scale() const
    {
	return m_plane->scale;
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

    KMSScreen::KMSScreen(bool primary)
    {
	m_fd = drmOpen("atmel-hlcdc", NULL);
	assert(m_fd >= 0);

	m_device = kms_device_open(m_fd);
	assert(m_device);

	//kms_device_dump(m_device);

	if (primary)
	{
	    m_plane = plane_create(m_device,
				   DRM_PLANE_TYPE_PRIMARY,
				   0,
				   m_device->screens[0]->width,
				   m_device->screens[0]->height,
				   DRM_FORMAT_XRGB8888);

	    assert(m_plane);
	    plane_fb_map(m_plane);
	    assert(m_plane->buf);

	    plane_apply(m_plane);

	    DBG("primary plane dumb buffer " << plane_width(m_plane) << "," <<
		plane_height(m_plane));

	    init(m_plane->buf, plane_width(m_plane), plane_height(m_plane));
	}

	the_kms = this;
    }

    KMSScreen* KMSScreen::instance()
    {
	return the_kms;
    }

    struct plane_data* KMSScreen::allocate_overlay(const Size& size, uint32_t format)
    {
	struct plane_data* plane = 0;

	// brute force: find something that will work
	for (int index = 0; index < 3; index++)
	{
	    plane = plane_create(m_device,
				 DRM_PLANE_TYPE_OVERLAY,
				 index,
				 size.w,
				 size.h,
				 format);
	    if (plane)
		break;
	}

	assert(plane);
	plane_fb_map(plane);
	assert(plane->buf);

	plane_set_pos(plane, 0, 0);

	cout << "plane " << plane->index << " overlay dumb buffer " << plane_width(plane) << "," <<
	    plane_height(plane) << endl;

	//plane_apply(plane);

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

    KMSScreen::~KMSScreen()
    {
	kms_device_close(m_device);
	drmClose(m_fd);
    }

}
#endif
