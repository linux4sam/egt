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

	virtual void blit(cairo_surface_t* surface,
			  int srcx, int srcy,
			  int srcw, int srch,
			  int dstx, int dsty,
			  bool blend = true);

	virtual void fill(const Color& color = Color::RED);

	virtual void flip(const std::vector<Rect>& damage);

	virtual void schedule_flip() {}
	virtual uint32_t index() { return 0; }

	virtual void text(const Point& p, const std::string& str);

	virtual void rect(const Rect& rect, const Color& color);

	Size size() const { return m_size; }

	shared_cairo_t context() const { return m_cr; }

	virtual ~IScreen();

    protected:

	void init(void** ptr, uint32_t count, int w, int h);

	shared_cairo_surface_t m_surface;
	shared_cairo_t m_cr;

	struct DisplayBuffer
	{
	    shared_cairo_surface_t surface;
	    shared_cairo_t cr;

	    /**
	     * Each region that needs to be copied from the back buffer.
	     */
	    std::vector<Rect> damage;

	    void add_damage(const Rect& rect)
	    {
		if (!rect.is_clear())
		{
		    for (auto i = damage.begin(); i != damage.end(); ++i)
		    {
			if (Rect::is_intersect(*i,rect))
			{
			    Rect merged(Rect::merge(*i,rect));
			    damage.erase(i);
			    add_damage(merged);
			    return;
			}
		    }

		    // if we get here, no intersect found so add it
		    damage.push_back(rect);
		}
	    }
	};

	std::vector<DisplayBuffer> m_buffers;

	void copy_to_buffer(DisplayBuffer& buffer);
	void copy_to_buffer_greenscreen(DisplayBuffer& buffer, const std::vector<Rect>& olddamage);

	Size m_size;
    };

    /**
     * Screen on a fbdev framebuffer.
     *
     * The framebuffer is mmap()'ed and directly accessible.
     */
    class FrameBuffer : public IScreen
    {
    public:

	FrameBuffer(const std::string& path = "/dev/fb0");
	virtual ~FrameBuffer();

    protected:
	int m_fd;
	void* m_fb;
    };

    IScreen* main_screen();
    void set_main_screen(IScreen* screen);
}

#endif
