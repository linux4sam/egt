/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#ifndef SCREEN_H
#define SCREEN_H

#include "geometry.h"
#include <cairo.h>
#include <cassert>
#include <fcntl.h>
#include <linux/fb.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <vector>

class Screen
{
public:
	Screen();

	void blit(cairo_surface_t* surface, int srcx, int srcy, int srcw, int srch, int dstx, int dsty);
	#if 0
	void blit_scaled(cairo_surface_t* surface, int srcx, int srcy, int srcw, int srch, int dstx, int dsty, double scale);
	#endif

	void fill();

	void flip(const std::vector<Rect>& damage);

	virtual ~Screen();

protected:

	void init(void* ptr, int w, int h);

	cairo_surface_t* m_surface_back;
	cairo_t* m_cr_back;

	cairo_surface_t* m_surface;
	cairo_t* m_cr;
};

class FrameBuffer : public Screen
{
public:
	FrameBuffer(const std::string& path = "/dev/fb0");

	virtual ~FrameBuffer();

private:
	int m_fd;
	void* m_fb;
};

Screen* screen();

#endif
