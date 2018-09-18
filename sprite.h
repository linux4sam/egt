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

	bool is_last_frame() const;

	void show_frame(int index);

	uint32_t frame_count() const { return m_strips[m_strip].framecount; }

	shared_cairo_surface_t surface() const;

	struct strip
	{
	    int framecount;
	    int framex;
	    int framey;
	};

	void set_strip(uint32_t id)
	{
	    if (id < m_strips.size() && id != m_strip)
	    {
		m_strip = id;
		m_index = 0;
	    }
	}

	uint32_t add_strip(int framecount, int framex, int framey)
	{
	    strip s;
	    s.framecount = framecount;
	    s.framex = framex;
	    s.framey = framey;
	    m_strips.push_back(s);
	    return m_strips.size()-1;
	}

	virtual ~HardwareSprite();

    protected:

	Image m_image;
	std::string m_filename;
	Size m_frame;
	int m_index;
	std::vector<strip> m_strips;
	uint32_t m_strip;
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
