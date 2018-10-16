/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_SPRITE_H
#define MUI_SPRITE_H

/**
 * @file
 * @brief Working with sprites.
 */

#include <mui/image.h>
#include <mui/widget.h>
#include <mui/window.h>

namespace mui
{

    /**
     * Base class for sprite functionality.
     */
    class ISpriteBase
    {
    public:
        ISpriteBase(const std::string& filename, int framew, int frameh,
                    int framecount, int framex = 0, int framey = 0)
            : m_image(filename),
              m_filename(filename),
              m_frame(framew, frameh),
              m_index(0)
        {
            m_strip = add_strip(framecount, framex, framey);
        }

        virtual void show_frame(int index) = 0;

        /**
         * Get a copy of a surface for the current frame.
         */
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
            return m_index >= m_strips[m_strip].framecount - 1;
        }

        virtual uint32_t frame_count() const
        {
            return m_strips[m_strip].framecount;
        }

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
            return m_strips.size() - 1;
        }

        virtual ~ISpriteBase()
        {}

    protected:

        virtual void get_frame_offsets(int index, int& panx, int& pany) const
        {
            int imagew = m_image.w();
            panx = m_strips[m_strip].framex + (index * m_frame.w);
            pany = m_strips[m_strip].framey;

            // support sheets that have frames on multiple rows
            if (panx + m_frame.w >= imagew)
            {
                int x = m_strips[m_strip].framex + (index * m_frame.w);

                panx = (x % imagew);
                pany = ((x / imagew) * m_strips[m_strip].framey) + (x / imagew) * m_frame.h;
            }
        }

        Image m_image;
        std::string m_filename;
        Size m_frame;
        int m_index;
        std::vector<strip> m_strips;
        uint32_t m_strip;
    };

    /**
     * Sprite widget using hardware planes.
     */
    class HardwareSprite : public PlaneWindow, public ISpriteBase
    {
    public:
        HardwareSprite(const std::string& filename, int framew, int frameh,
                       int framecount, int framex = 0, int framey = 0,
                       const Point& point = Point());

        virtual void show_frame(int index);

        virtual ~HardwareSprite();
    };

    /**
     * Sprite widget using only software.
     */
    class SoftwareSprite : public Widget, public ISpriteBase
    {
    public:
        SoftwareSprite(const std::string& filename, int framew, int frameh,
                       int framecount, int framex = 0, int framey = 0,
                       const Point& point = Point());

        virtual void draw(Painter& painter, const Rect& rect);

        virtual void show_frame(int index);

        virtual ~SoftwareSprite();
    };

}

#endif
