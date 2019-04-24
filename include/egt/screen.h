/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SCREEN_H
#define EGT_SCREEN_H

/**
 * @file
 * @brief Working with screens.
 */

#include <cairo.h>
#include <deque>
#include <memory>
#include <egt/geometry.h>
#include <egt/types.h>
#include <vector>
#include <iosfwd>

namespace egt
{
inline namespace v1
{

/**
 * @brief Supported pixel formats.
 *
 * @note Some backends may not support all formats.
 */
enum class pixel_format
{
    invalid,
    rgb565,
    argb8888,
    xrgb8888,
    yuyv,
    nv21,
    yuv420,
};

std::ostream& operator<<(std::ostream& os, const pixel_format& format);

/**
 * @brief Hidden detail namespace.
 *
 * The detail namespace is considered unstable and private to EGT. use at your
 * own risk.
 */
namespace detail
{
cairo_format_t cairo_format(pixel_format format);
uint32_t drm_format(pixel_format format);
pixel_format egt_format(uint32_t format);
pixel_format egt_format(cairo_format_t format);
}

/**
 * @brief Manages one of more buffers that make up a surface.
 *
 * @warning A Screen is not necissarily the same resolution and orientation of
 * the Display.
 */
class Screen
{
public:

    /**
     * Type used for damage arrays.
     */
    using damage_array = std::deque<Rect>;

    Screen();

    /**
     * Perform a flip of the buffers.
     *
     * @note This will call schedule_flip() automatically.
     */
    virtual void flip(const damage_array& damage);

    /**
     * Schedule a flip to occur later.
     */
    virtual void schedule_flip()  = 0;

    /**
     * If the screen implementation manages multiple buffers, this will
     * return the index of the current buffer.
     */
    virtual uint32_t index() { return 0; }

    /**
     * Size of the screen.
     */
    Size size() const { return m_size; }

    /**
     * Bounding box for the screen.
     */
    Rect box() const { return Rect(Point(), m_size); }

    /**
     * Get the context for the screen.
     *
     * Get the target surface from this using cairo_get_target().
     */
    shared_cairo_t context() const { return m_cr; }

    /**
     * This function implements the algorithm for adding damage rectangles
     * to a list.
     */
    static void damage_algorithm(Screen::damage_array& damage,
                                 const Rect& rect);

    virtual ~Screen();

protected:

    void init(void** ptr, uint32_t count, int w, int h,
              pixel_format format = pixel_format::argb8888);

    /// @private
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

            Screen::damage_algorithm(damage, rect);
        }

        inline ~DisplayBuffer() noexcept {}
    };

    void copy_to_buffer(DisplayBuffer& buffer);
    void copy_to_buffer_greenscreen(DisplayBuffer& buffer,
                                    const damage_array& olddamage);

    shared_cairo_surface_t m_surface;
    shared_cairo_t m_cr;

    using buffer_array = std::vector<DisplayBuffer>;

    buffer_array m_buffers;
    Size m_size;
};

/**
 * Get a pointer to the main screen.
 */
Screen*& main_screen();

}
}

#endif
