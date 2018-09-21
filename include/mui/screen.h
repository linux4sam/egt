/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_SCREEN_H
#define MUI_SCREEN_H

#include "mui/color.h"
#include "mui/geometry.h"
#include <cairo.h>
#include <vector>
#include <memory>
#include <drm_fourcc.h>

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

        virtual void rect(const Rect& rect, const Color& color);

        Size size() const { return m_size; }

        shared_cairo_t context() const { return m_cr; }

        virtual ~IScreen();

    protected:

        void init(void** ptr, uint32_t count, int w, int h,
		  uint32_t f = DRM_FORMAT_ARGB8888);

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
                if (!rect.empty())
                {
                    for (auto i = damage.begin(); i != damage.end(); ++i)
                    {
                        if (Rect::is_intersect(*i, rect))
                        {
                            Rect super(Rect::merge(*i, rect));
#if 1
                            /*
                             * if the area of the two rectangles minus their
                             * intersection area is smaller than the area of the super
                             * rectangle, then don't merge
                             */
                            Rect intersect(Rect::intersect(*i, rect));
                            if (((*i).area() + rect.area() - intersect.area()) < super.area())
                            {
                                break;
                            }
#endif
                            damage.erase(i);
                            add_damage(super);
                            return;
                        }
                    }

                    // if we get here, no intersect found so add it
                    damage.push_back(rect);
                }
            }
        };

        void copy_to_buffer(DisplayBuffer& buffer);
        void copy_to_buffer_greenscreen(DisplayBuffer& buffer, const std::vector<Rect>& olddamage);

        shared_cairo_surface_t m_surface;
        shared_cairo_t m_cr;
        std::vector<DisplayBuffer> m_buffers;
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
