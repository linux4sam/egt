/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef KMSSCREEN_H
#define KMSSCREEN_H

#include "screen.h"
#include "window.h"
#include "geometry.h"

#ifdef HAVE_LIBPLANES
#include <planes/kms.h>

struct plane_data;
struct kms_device;

namespace mui
{

    /**
     * A screen backed by an overlay plane.
     */
    class KMSOverlayScreen : public IScreen
    {
    public:

	KMSOverlayScreen(struct plane_data* plane);

	virtual void position(int x, int y);

	void apply();

	virtual ~KMSOverlayScreen();

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

	struct plane_data* allocate_overlay(const Size& size);

	/**
	 *
	 */
	uint32_t count_planes(int type = DRM_PLANE_TYPE_OVERLAY);

	virtual ~KMSScreen();

	static KMSScreen* instance();

    protected:
	int m_fd;
	struct kms_device* m_device;
    };

}
#endif

#endif
