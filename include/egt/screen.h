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
#include <egt/geometry.h>
#include <egt/types.h>
#include <egt/utils.h>
#include <iosfwd>
#include <memory>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * Manages one of more buffers that make up a Screen.
 *
 * @note A Screen is not necessarily the same resolution and orientation of
 * the Display.
 */
class Screen : public detail::noncopyable
{
public:

    /**
     * Type used for damage arrays.
     */
    using damage_array = std::vector<Rect>;

    Screen();

    /**
     * Perform a flip of the buffers.
     *
     * This iterates the buffers and puts the composition buffer into the screen
     * buffers.
     *
     * @note This will call schedule_flip() automatically.
     */
    virtual void flip(const damage_array& damage);

    /**
     * Schedule a flip to occur later.
     *
     * This is needed if a flip should to occur sometime in the future to the
     * hardware.
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
    inline Size size() const { return m_size; }

    /**
     * Bounding box for the screen.
     */
    inline Rect box() const { return Rect(Point(), m_size); }

    /**
     * Get the context for the screen.
     *
     * Get the target surface from this using cairo_get_target().
     */
    shared_cairo_t context() const { return m_cr; }

    /**
     * This function implements the algorithm for adding damage rectangles
     * to a list.
     *
     * @param[in,out] damage The starting and ending damage array.
     * @param[in] rect The new rectangle to add.
     *
     * @note This function may be implemented to be recursive.
     */
    static void damage_algorithm(Screen::damage_array& damage, Rect rect);

    /**
     * Set if asynchronous buffer flips are used.
     */
    inline void set_async_flip(bool async)
    {
        m_async = async;
    }

    /**
     * Get the max brightness of the screen.
     *
     * @note Not all screens support this capability.
     */
    virtual size_t max_brightness() const;

    /**
     * Get the current brightness of the screen.
     *
     * @note Not all screens support this capability.
     */
    virtual size_t brightness() const;

    /**
     * Set the brightness of the screen.
     *
     * @param brightness Value from 0 to max_brightness().
     *
     * @note Not all screens support this capability.
     */
    virtual void set_brightness(size_t brightness);

    /**
     * Configure low fidelity options.
     *
     * This configures settings related to font hinting, font aliasing, and
     * shape aliasing.
     */
    virtual void low_fidelity();

    /**
     * Configure high fidelity options.
     *
     * This configures settings related to font hinting, font aliasing, and
     * shape aliasing.
     */
    virtual void high_fidelity();

    virtual ~Screen();

protected:

    void init(void** ptr, uint32_t count, const Size& size,
              pixel_format format = pixel_format::argb8888);

    inline void init(const Size& size, pixel_format format = pixel_format::argb8888)
    {
        init(nullptr, 0, size, format);
    }

    /// @private
    struct ScreenBuffer
    {
        explicit ScreenBuffer(cairo_surface_t* s) noexcept
            : surface(s)
        {
            damage.reserve(10);
        }

        unique_cairo_surface_t surface;

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
    };

    virtual void copy_to_buffer(ScreenBuffer& buffer);

    void copy_to_buffer_software(ScreenBuffer& buffer);

    /**
     * Composition surface.
     */
    shared_cairo_surface_t m_surface;

    /**
     * Composition surface context.
     */
    shared_cairo_t m_cr;

    /**
     * Type used for an array of ScreenBuffer objects.
     */
    using buffer_array = std::vector<ScreenBuffer>;

    /**
     * Screen buffer array.
     */
    buffer_array m_buffers;

    /**
     * Size of the screen.
     */
    Size m_size;

    bool m_async{false};
};

}
}

#endif
