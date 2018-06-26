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
#ifdef HAVE_IMLIB2
#include <Imlib2.h>
#endif

using namespace std;

#define BACK_BUFFER

#ifdef HAVE_IMLIB2
//#define USE_IMLIB2
#endif

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

    static inline uint32_t blendPreMulAlpha2(uint32_t p1, uint32_t p2)
    {
	static const int AMASK = 0xFF000000;
	static const int RBMASK = 0x00FF00FF;
	static const int GMASK = 0x0000FF00;
	static const int AGMASK = AMASK | GMASK;
	static const int ONEALPHA = 0x01000000;
	unsigned int a = (p2 & AMASK) >> 24;
	unsigned int na = 255 - a;
	unsigned int rb = ((na * (p1 & RBMASK)) + (a * (p2 & RBMASK))) >> 8;
	unsigned int ag = (na * ((p1 & AGMASK) >> 8)) +
	    (a * (ONEALPHA | ((p2 & GMASK) >> 8)));
	return ((rb & RBMASK) | (ag & AGMASK));
    }

    void IScreen::blit(cairo_surface_t* surface, int srcx, int srcy, int srcw,
		       int srch, int dstx, int dsty, bool blend)
    {
#ifdef USE_IMLIB2
	cairo_surface_flush(surface);
	cairo_surface_flush(m_surface.get());

	uint32_t* src = (uint32_t*)cairo_image_surface_get_data(surface);
	uint32_t* dst = (uint32_t*)cairo_image_surface_get_data(m_surface.get());

	const int sw = cairo_image_surface_get_width(surface);
	const int sh = cairo_image_surface_get_height(surface);
	const int ss = sw * sh;
	const int dw = cairo_image_surface_get_width(m_surface.get());
	const int dh = cairo_image_surface_get_height(m_surface.get());
	const int ds = dw * dh;

	Imlib_Image image;
	image = imlib_create_image_using_data(sw,sh,src);
	assert(image);
	imlib_context_set_image(image);
	imlib_image_set_has_alpha(1);

	Imlib_Image buffer;
	buffer = imlib_create_image_using_data(dw,dh,dst);
	assert(buffer);

	imlib_context_set_image(buffer);

	//imlib_image_set_format();
	imlib_context_set_blend(1);
	//imlib_blend_image_onto_image(image, 0,
	//			     srcx, srcy, srcw, srch,
	//			     dstx, dsty, srcw, srch);
	imlib_blend_image_onto_image(image, 0,
				     0, 0, sw, sh,
				     dstx, dsty, sw, sh);
	imlib_free_image();
	imlib_context_set_image(image);
	imlib_free_image();

	cairo_surface_mark_dirty(m_surface.get());
#else
	if (true || !blend)
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
	else
	{
	    cairo_surface_flush(surface);
	    cairo_surface_flush(m_surface.get());

	    uint32_t* src = (uint32_t*)cairo_image_surface_get_data(surface);
	    uint32_t* dst = (uint32_t*)cairo_image_surface_get_data(m_surface.get());

	    const int sw = cairo_image_surface_get_width(surface);
	    const int sh = cairo_image_surface_get_height(surface);
	    //const int ss = sw * sh;
	    const int dw = cairo_image_surface_get_width(m_surface.get());
	    const int dh = cairo_image_surface_get_height(m_surface.get());
	    const int ds = dw * dh;

	    const int w = srcx + srcw;
	    const int h = srcy + srch;
	    for (int y = srcy; y < h;y++)
	    {
		const int doff_ = y * dw;
		const int sy = y - dsty;
		const int soff_ = sy * sw;

		if (sy < 0 || sy >= sh)
		    continue;

		for (int x = srcx; x < w;x++)
		{
		    const int sx = x - dstx;

		    if (sx < 0 || sx >= sw)
		    	continue;

		    const int doff = doff_ + x;

		    if (doff < 0 || doff >= ds)
		    	continue;

		    const int soff = soff_ + sx;

#define SBUF (src + soff)
#define DBUF (dst + doff)

		    //if (((*SBUF >> 24) & 0xff) == 255)
		    //{
		    //*DBUF = *SBUF;
		    //}
		    //else
		    //{
		    uint32_t* d = DBUF;
		    *d = blendPreMulAlpha2(*d, *SBUF);

		    //}
		}
	    }

	    cairo_surface_mark_dirty(m_surface.get());
	}
#endif
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
#ifdef BACK_BUFFER
	//cout << "flip" << endl;
	//greenscreen(damage);
#ifndef USE_IMLIB2
	//int total = 0;
	//cairo_save(m_cr_back.get());
	cairo_set_source_surface(m_cr_back.get(), m_surface.get(), 0, 0);
	cairo_set_operator(m_cr_back.get(), CAIRO_OPERATOR_SOURCE);

	for (const auto& d: damage)
	{
	    //total += (d.w * d.h);
	    cairo_rectangle(m_cr_back.get(), d.x, d.y, d.w, d.h);
	}
	cairo_fill(m_cr_back.get());
	//cairo_restore(m_cr_back.get());
#else
	uint32_t* src = (uint32_t*)cairo_image_surface_get_data(m_surface.get());
	uint32_t* dst = (uint32_t*)cairo_image_surface_get_data(m_surface_back.get());

	const int sw = cairo_image_surface_get_width(m_surface.get());
	const int sh = cairo_image_surface_get_height(m_surface.get());
	const int ss = sw * sh;
	const int dw = cairo_image_surface_get_width(m_surface_back.get());
	const int dh = cairo_image_surface_get_height(m_surface_back.get());
	const int ds = dw * dh;

	Imlib_Image image;
	image = imlib_create_image_using_data(sw,sh,src);
	assert(image);
	imlib_context_set_image(image);
	imlib_image_set_has_alpha(0);

	Imlib_Image buffer;
	buffer = imlib_create_image_using_data(dw,dh,dst);
	assert(buffer);

	imlib_context_set_image(buffer);
	imlib_context_set_blend(0);
	for (const auto& d: damage)
	{
	    imlib_blend_image_onto_image(image, 0,
					 d.x, d.y, d.w, d.h,
					 d.x, d.y, d.w, d.h);
	}

	imlib_free_image();
	imlib_context_set_image(image);
	imlib_free_image();

	cairo_surface_mark_dirty(m_surface_back.get());
#endif

	cairo_surface_flush(m_surface_back.get());

	//cout << "total pixels: " << total << endl;
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

#ifdef BACK_BUFFER
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

	    //m_surface = shared_cairo_surface_t(cairo_surface_create_similar(m_surface_back.get(), CAIRO_CONTENT_COLOR_ALPHA, w, h),
	    //				       cairo_surface_destroy);

	    cairo_format_t format_surface = CAIRO_FORMAT_ARGB32;

	    m_surface = shared_cairo_surface_t(cairo_image_surface_create(format_surface, w, h),
					       cairo_surface_destroy);
	}
	else
	{
	    cairo_format_t format_surface = CAIRO_FORMAT_ARGB32;

	    m_surface = shared_cairo_surface_t(cairo_image_surface_create(format_surface, w, h),
					       cairo_surface_destroy);
	}

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

    // https://github.com/toradex/cairo-fb-examples/blob/master/rectangles/rectangles.c

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

	cout << "fb size " << fixinfo.smem_len << " " << varinfo.xres << "," << varinfo.yres << endl;

	m_fb = mmap(NULL, fixinfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
	assert(m_fb != (void *) -1);

	init(m_fb, varinfo.xres, varinfo.yres);
	//init(m_fb, varinfo.xres, varinfo.yres_virtual);
    }

    FrameBuffer::~FrameBuffer()
    {
	::munmap(m_fb,0);
	::close(m_fd);
    }

}
