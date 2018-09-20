/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef SPRITE_H
#define SPRITE_H

#include "mui/window.h"
#include "mui/widget.h"

namespace mui
{

    class SpriteBase
    {
    public:
	SpriteBase(const std::string& filename, int framew, int frameh,
		   int framecount, int framex = 0, int framey = 0,
		   int x = 0, int y = 0)
	    : m_image(filename),
	      m_filename(filename),
	      m_frame(framew,frameh),
	      m_index(0)
	{
	    m_strip = add_strip(framecount, framex, framey);
	}

	virtual void show_frame(int index) = 0;

	virtual shared_cairo_surface_t surface() const;

	virtual void advance()
	{
	    int index = m_index;
	    if (++index >= m_strips[m_strip].framecount)
		index = 0;

	    show_frame(index);
	}

	virtual bool is_last_frame() const
	{
	    return m_index >= m_strips[m_strip].framecount-1;
	}

	virtual uint32_t frame_count() const { return m_strips[m_strip].framecount; }

	struct strip
	{
	    int framecount;
	    int framex;
	    int framey;
	};

	virtual void set_strip(uint32_t id)
	{
	    if (id < m_strips.size() && id != m_strip)
	    {
		m_strip = id;
		m_index = 0;
	    }
	}

	virtual uint32_t add_strip(int framecount, int framex, int framey)
	{
	    strip s;
	    s.framecount = framecount;
	    s.framex = framex;
	    s.framey = framey;
	    m_strips.push_back(s);
	    return m_strips.size()-1;
	}

	virtual ~SpriteBase()
	{}

    protected:
	Image m_image;
	std::string m_filename;
	Size m_frame;
	int m_index;
	std::vector<strip> m_strips;
	uint32_t m_strip;
    };

    class HardwareSprite : public PlaneWindow, public SpriteBase
    {
    public:
	HardwareSprite(const std::string& filename, int framew, int frameh,
		       int framecount, int framex = 0, int framey = 0,
		       int x = 0, int y = 0);

	virtual	void damage() {}

	virtual void damage(const Rect& rect) {}

	void show_frame(int index);

	virtual ~HardwareSprite();
    };

    class SoftwareSprite : public Widget, public SpriteBase
    {
    public:
	SoftwareSprite(const std::string& filename, int framew, int frameh,
		       int framecount, int framex = 0, int framey = 0,
		       int x = 0, int y = 0);

	virtual void draw(const Rect& rect);

	void show_frame(int index);

	virtual ~SoftwareSprite();
    };

}

#endif
