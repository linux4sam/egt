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

namespace mui
{

    static IScreen* the_screen = 0;

    IScreen* main_screen()
    {
	return the_screen;
    }

    IScreen::IScreen()
    {

    }

#if 0
    void IScreen::blit_scaled(cairo_surface_t* surface, int srcx, int srcy, int srcw, int srch, int dstx, int dsty, double scale)
    {
	cairo_matrix_t matrix;

	cairo_save(m_cr);
	cairo_get_matrix(m_cr, &matrix);

	/* Scale *before* setting the source surface (1) */
	cairo_scale(m_cr, scale, scale);
	cairo_set_source_surface(m_cr, surface, dstx, dsty);
	cairo_rectangle(m_cr, srcx, srcy, srcw, srch);

	/* To avoid getting the edge pixels blended with 0 alpha, which would
	 * occur with the default EXTEND_NONE. Use EXTEND_PAD for 1.2 or newer (2)
	 */
	cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REFLECT);

	cairo_set_operator(m_cr, CAIRO_OPERATOR_OVER);

	cairo_fill(m_cr);

	cairo_scale(m_cr, 1.0, 1.0);
	cairo_set_matrix(m_cr, &matrix);
	cairo_restore(m_cr);
    }
#endif

    void IScreen::blit(cairo_surface_t* surface, int srcx, int srcy, int srcw,
		       int srch, int dstx, int dsty, bool blend)
    {
	cairo_save(m_cr.get());
	cairo_set_source_surface(m_cr.get(), surface, dstx, dsty);
	cairo_rectangle(m_cr.get(), srcx, srcy, srcw, srch);
	if (blend)
	    cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_OVER);
	else
	    cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_SOURCE);
	cairo_fill(m_cr.get());
	cairo_restore(m_cr.get());

	assert(cairo_status(m_cr.get()) == CAIRO_STATUS_SUCCESS);
    }

    void IScreen::fill(const Color& color)
    {
	cairo_save(m_cr.get());
	cairo_set_source_rgba(m_cr.get(),
			     color.redf(),
			     color.greenf(),
			      color.bluef(),
			      color.alphaf());
	cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_SOURCE);
	cairo_paint(m_cr.get());
	cairo_restore(m_cr.get());
    }

    void IScreen::rect(const Rect& rect, const Color& color)
    {
	cairo_save(m_cr.get());
	cairo_set_source_rgba(m_cr.get(),
			      color.redf(),
			      color.greenf(),
			      color.bluef(),
			      color.alphaf());
	if (color.alpha() == 255)
	    cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_SOURCE);
	else
	    cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_OVER);
	cairo_rectangle(m_cr.get(), rect.x, rect.y, rect.w, rect.h);
	cairo_fill(m_cr.get());
	cairo_restore(m_cr.get());
    }

    void IScreen::greenscreen(const vector<Rect>& damage)
    {
	Color color = Color::GREEN;

	cairo_save(m_cr_back.get());
	cairo_set_source_surface(m_cr_back.get(), m_surface.get(), 0, 0);
	cairo_set_operator(m_cr_back.get(), CAIRO_OPERATOR_SOURCE);
	cairo_set_source_rgba(m_cr_back.get(), color.redf(), color.greenf(), color.bluef(), color.alphaf());
	cairo_set_line_width(m_cr_back.get(), 1.0);
	for (const auto& d: damage)
	{
	    cairo_rectangle(m_cr_back.get(), d.x+1, d.y+1, d.w-2, d.h-2);
	}
	cairo_stroke(m_cr_back.get());
	cairo_restore(m_cr_back.get());

	cairo_surface_flush(m_surface_back.get());

	struct timespec ts = {0,300 * 1000000};
	nanosleep(&ts, NULL);
    }

    void IScreen::flip(const vector<Rect>& damage)
    {
	//greenscreen(damage);

#ifdef DOUBLE_BUFFER
#if 0 // whole thing
	cairo_save(m_cr_back.get());
	cairo_set_source_surface(m_cr_back.get(), m_surface.get(), 0, 0);
	cairo_set_operator(m_cr_back.get(), CAIRO_OPERATOR_SOURCE);
	cairo_paint(m_cr_back.get());
	cairo_restore(m_cr_back.get());
#else
	int total = 0;
	cairo_save(m_cr_back.get());
	cairo_set_source_surface(m_cr_back.get(), m_surface.get(), 0, 0);
	cairo_set_operator(m_cr_back.get(), CAIRO_OPERATOR_SOURCE);
	for (const auto& d: damage)
	{
	    total += (d.w * d.h);
	    cairo_rectangle(m_cr_back.get(), d.x, d.y, d.w, d.h);
	}
	cairo_fill(m_cr_back.get());
	cairo_restore(m_cr_back.get());

	cairo_surface_flush(m_surface_back.get());

	//cout << "total pixels: " << total << endl;

#endif

#endif
    }

    void IScreen::text(const Point& p, const std::string& str)
    {
	cairo_text_extents_t textext;
	cairo_select_font_face(m_cr.get(), "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(m_cr.get(), 15);
	cairo_text_extents(m_cr.get(), "a", &textext);

	cairo_move_to(m_cr.get(), textext.width - textext.x_bearing, textext.height - textext.y_bearing);
	cairo_show_text(m_cr.get(), str.c_str());
    }

    IScreen::~IScreen()
    {
    }

    void IScreen::init(void* ptr, int w, int h)
    {
	cout << "screen " << w << "," << h << endl;

	m_size = Size(w,h);

	cairo_format_t format = CAIRO_FORMAT_ARGB32;
	//cairo_format_t format = CAIRO_FORMAT_RGB16_565;
	//cairo_format_t format = CAIRO_FORMAT_RGB24;

#ifdef DOUBLE_BUFFER
	if (ptr)
	{
	    m_surface_back = shared_cairo_surface_t(cairo_image_surface_create_for_data((unsigned char*)ptr,
											format,
											w, h,
											cairo_format_stride_for_width(format, w)),
						    cairo_surface_destroy);
	    assert(m_surface_back);

	    m_cr_back = shared_cairo_t(cairo_create(m_surface_back.get()), cairo_destroy);
	    assert(m_cr_back);
	}

	cairo_format_t format_surface = CAIRO_FORMAT_ARGB32;

	m_surface = shared_cairo_surface_t(cairo_image_surface_create(format_surface, w, h),
					   cairo_surface_destroy);
	assert(m_surface.get());

	m_cr = shared_cairo_t(cairo_create(m_surface.get()), cairo_destroy);
	assert(m_cr);
#else
	m_surface = shared_cairo_surface_t(cairo_image_surface_create_for_data((unsigned char*)ptr,
									       format,
									       w, h,
									       cairo_format_stride_for_width(format, w)),
					   cairo_surface_destroy);
	assert(m_surface.get());

	m_cr = shared_cairo_t(cairo_create(m_surface.get()), cairo_destroy);
	assert(m_cr);
#endif

	if (!the_screen)
	    the_screen = this;
    }

    FrameBuffer::FrameBuffer(const string& path)
	: m_fd(-1)
    {
	struct fb_fix_screeninfo fixinfo;
	struct fb_var_screeninfo varinfo;

	cout << path << endl;
	m_fd = open(path.c_str(), O_RDWR);
	assert(m_fd >= 0);

	if (ioctl(m_fd, FBIOGET_FSCREENINFO, &fixinfo) < 0)
	    assert(0);

	if (ioctl(m_fd, FBIOGET_VSCREENINFO, &varinfo) < 0)
	    assert(0);

	int size = fixinfo.smem_len;
	//int size = varinfo.xres * varinfo.yres * varinfo.bits_per_pixel / 8;

	cout << "fb size " << size << " " << varinfo.xres << "," << varinfo.yres << endl;

	m_fb = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
	assert(m_fb != (void *) -1);

	init(m_fb, varinfo.xres, varinfo.yres);
    }

    FrameBuffer::~FrameBuffer()
    {
	::munmap(m_fb,0);
	::close(m_fd);
    }

}
