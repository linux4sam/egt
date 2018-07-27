/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef SPRITE_H
#define SPRITE_H

#include "window.h"
#include "widget.h"

namespace mui
{

    class HardwareSprite : public PlaneWindow
    {
    public:
	HardwareSprite(const std::string& filename, int framew, int frameh,
		       int framecount, int framex = 0, int framey = 0,
		       int x = 0, int y = 0);

	virtual	void damage() {}

	virtual void damage(const Rect& rect) {}

	void advance();

	void show_frame(int index);

	uint32_t frame_count() const { return m_framecount; }

	virtual ~HardwareSprite();

    protected:

	Image m_image;
	std::string m_filename;
	int m_framex;
	int m_framey;
	Size m_frame;
	int m_index;
	int m_framecount;
    };


    class SoftwareSprite : public Widget
    {
    public:
	SoftwareSprite(const std::string& filename, int framew, int frameh,
		       int framecount, int framex = 0, int framey = 0,
		       int x = 0, int y = 0);

	virtual void draw(const Rect& rect);

	void advance();

	void show_frame(int index);

	uint32_t frame_count() const { return m_framecount; }

	virtual ~SoftwareSprite();

    protected:

	shared_cairo_surface_t m_image;
	std::string m_filename;
	int m_framex;
	int m_framey;
	Size m_frame;
	int m_index;
	int m_framecount;
    };

}

#endif
