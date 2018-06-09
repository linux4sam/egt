/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */

#include "screen.h"
#include <sys/mman.h>
#include <cassert>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <iostream>

using namespace std;

#define DOUBLE_BUFFER

static Screen* the_screen = 0;

Screen* screen()
{
	return the_screen;
}

Screen::Screen()
{

}

#if 0
void Screen::blit_scaled(cairo_surface_t* surface, int srcx, int srcy, int srcw, int srch, int dstx, int dsty, double scale)
{
	//cairo_matrix_t matrix;

	cairo_save(m_cr);
	//cairo_get_matrix(m_cr, &matrix);

	/* Scale *before* setting the source surface (1) */
	cairo_scale(m_cr, scale, scale);
	cairo_set_source_surface(m_cr, surface, dstx, dsty);
	cairo_rectangle(m_cr, srcx, srcy, srcw, srch);

	/* To avoid getting the edge pixels blended with 0 alpha, which would
	 * occur with the default EXTEND_NONE. Use EXTEND_PAD for 1.2 or newer (2)
	 */
	//cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REFLECT);

	/* Replace the destination with the source instead of overlaying */
	//cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_set_operator(m_cr, CAIRO_OPERATOR_OVER);

	cairo_fill(m_cr);

	//cairo_scale(m_cr, 1.0, 1.0);
	//cairo_set_matrix(m_cr, &matrix);
	cairo_restore(m_cr);
}
#endif

void Screen::blit(cairo_surface_t* surface, int srcx, int srcy, int srcw, int srch, int dstx, int dsty)
{
	cairo_save(m_cr);
	cairo_set_source_surface(m_cr, surface, dstx, dsty);
	cairo_rectangle(m_cr, srcx, srcy, srcw, srch);
	//cairo_clip(m_cr);
	cairo_set_operator(m_cr, CAIRO_OPERATOR_OVER);
	cairo_fill(m_cr);
	cairo_restore(m_cr);
}

void Screen::fill()
{
	cairo_save(m_cr);
	cairo_set_source_rgba(m_cr, 0, 0, 1.00, 1.0);
	cairo_set_operator(m_cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(m_cr);
	cairo_restore(m_cr);
}

void Screen::flip(const vector<Rect>& damage)
{
#ifdef DOUBLE_BUFFER
	int total = 0;
	cairo_save(m_cr_back);
	cairo_set_source_surface(m_cr_back, m_surface, 0, 0);
	for (auto& d: damage)
	{
		total += (d.w * d.h);
		cairo_rectangle(m_cr_back, d.x, d.y, d.w, d.h);
		cairo_set_operator(m_cr_back, CAIRO_OPERATOR_SOURCE);
		cairo_fill(m_cr_back);
	}
	cairo_restore(m_cr_back);

	cout << "total pixels: " << total << endl;

#if 0 // whole thing
	cairo_save(m_cr_back);
	cairo_set_source_surface(m_cr_back, m_surface, 0, 0);
	cairo_set_operator(m_cr_back, CAIRO_OPERATOR_SOURCE);
	cairo_paint(m_cr_back);
	cairo_restore(m_cr_back);
#endif

#endif
}

Screen::~Screen()
{
	cairo_surface_destroy(m_surface);
	cairo_destroy(m_cr);
}

void Screen::init(void* ptr, int w, int h)
{
	cout << "framebuffer " << w << "," << h << endl;

	cairo_format_t format = CAIRO_FORMAT_ARGB32;
	//cairo_format_t format = CAIRO_FORMAT_RGB16_565;
	//cairo_format_t format = CAIRO_FORMAT_RGB24;

#ifdef DOUBLE_BUFFER
	m_surface_back = cairo_image_surface_create_for_data((unsigned char*)ptr,
							format,
							w, h,
							cairo_format_stride_for_width(format, w));
	assert(m_surface_back);

	m_cr_back = cairo_create(m_surface_back);
	assert(m_cr_back);

	m_surface = cairo_image_surface_create(format, w, h);
	assert(m_surface);

	m_cr = cairo_create(m_surface);
	assert(m_cr);
#else
	m_surface = cairo_image_surface_create_for_data((unsigned char*)ptr,
							format,
							w, h,
							cairo_format_stride_for_width(format, w));
	assert(m_surface);

	m_cr = cairo_create(m_surface);
	assert(m_cr);
#endif

	the_screen = this;

	fill();
}

FrameBuffer::FrameBuffer(const string& path)
	: m_fd(-1)
{
	struct fb_fix_screeninfo fixinfo;
	struct fb_var_screeninfo varinfo;

	cout << path << endl;
	m_fd = open(path.c_str(), O_RDWR);
	assert(m_fd >= 0);

	int ret = ioctl(m_fd, FBIOGET_FSCREENINFO, &fixinfo);
	assert(ret >= 0);

	ret  = ioctl(m_fd, FBIOGET_VSCREENINFO, &varinfo);
	assert(ret >= 0);

	//int size = fixinfo.smem_len;
	int size = varinfo.xres * varinfo.yres * varinfo.bits_per_pixel / 8;

	cout << "fb size " << size << endl;

	m_fb = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
	assert(m_fb != (void *) -1);

	init(m_fb, varinfo.xres, varinfo.yres);
}

FrameBuffer::~FrameBuffer()
{
	cairo_surface_destroy(m_surface);
	cairo_destroy(m_cr);

	cairo_surface_destroy(m_surface_back);
	cairo_destroy(m_cr_back);

	::munmap(m_fb,0);
	::close(m_fd);
}
