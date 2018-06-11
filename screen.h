/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef SCREEN_H
#define SCREEN_H

#include "color.h"
#include "geometry.h"
#include <cairo.h>
#include <vector>
#include <memory>

namespace mui
{

    using shared_cairo_surface_t =
	std::shared_ptr<cairo_surface_t>;

    using shared_cairo_t =
	std::shared_ptr<cairo_t>;

    /**
     * Base screen class.
     */
    class IScreen
    {
    public:
	IScreen();

	virtual void blit(cairo_surface_t* surface, int srcx, int srcy, int srcw, int srch, int dstx, int dsty, bool blend = true);

	virtual void fill(const Color& color = Color::RED);

	virtual void flip(const std::vector<Rect>& damage);

	virtual void text(const Point& p, const std::string& str);

	virtual void rect(const Rect& rect, const Color& color);

	Size size() const { return m_size; }

	shared_cairo_t context() const { return m_cr; }

	virtual ~IScreen();

    protected:

	void greenscreen(const std::vector<Rect>& damage);
	void init(void* ptr, int w, int h);

	shared_cairo_surface_t m_surface_back;
	shared_cairo_t m_cr_back;

	shared_cairo_surface_t m_surface;
	shared_cairo_t m_cr;

	Size m_size;
    };

    /**
     * Screen on a fbdev framebuffer.
     */
    class FrameBuffer : public IScreen
    {
    public:

	FrameBuffer(const std::string& path = "/dev/fb0");

	virtual ~FrameBuffer();

    private:
	int m_fd;
	void* m_fb;
    };

    IScreen* screen();

}

#endif
