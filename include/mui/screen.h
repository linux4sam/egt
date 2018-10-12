/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_SCREEN_H
#define MUI_SCREEN_H

/**
 * @file
 * @brief Working with screens.
 */

#include <cairo.h>
#include <deque>
#include <drm_fourcc.h>
#include <memory>
#include <mui/geometry.h>
#include <vector>

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
        using damage_array = std::deque<Rect>;

        IScreen();

        virtual void flip(const damage_array& damage);

        virtual void schedule_flip() {}

        virtual uint32_t index() { return 0; }

        Size size() const { return m_size; }

        shared_cairo_t context() const { return m_cr; }

        virtual ~IScreen();

        /**
         * This function implements the algorithm for adding damages rectangles
         * to a list.
         */
        static void damage_algorithm(IScreen::damage_array& damage,
                                     const Rect& rect);

    protected:

        void init(void** ptr, uint32_t count, int w, int h,
                  uint32_t f = DRM_FORMAT_ARGB8888);

        struct DisplayBuffer
        {
            shared_cairo_surface_t surface;
            shared_cairo_t cr;

            /**
             * Each rect that needs to be copied from the back buffer.
             */
            damage_array damage;

            void add_damage(const Rect& rect)
            {
                if (rect.empty())
                    return;

                IScreen::damage_algorithm(damage, rect);
            }

            inline ~DisplayBuffer() noexcept {}
        };

        void copy_to_buffer(DisplayBuffer& buffer);
        void copy_to_buffer_greenscreen(DisplayBuffer& buffer,
                                        const damage_array& olddamage);

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

    /**
     * Get a pointer to the main screen.
     */
    IScreen*& main_screen();
}

#endif
