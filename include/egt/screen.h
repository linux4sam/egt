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

#include <egt/detail/meta.h>
#include <egt/geometry.h>
#include <egt/painter.h>
#include <egt/surface.h>
#include <egt/types.h>
#include <iosfwd>
#include <memory>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace detail
{
class FrameBufferInfo;
}

/**
 * Manages one of more buffers that make up a Screen.
 *
 * @note A Screen is not necessarily the same resolution and Orientation of
 * the Display.
 */
class EGT_API Screen
{
public:

    /**
     * Type used for damage arrays.
     */
    using DamageArray = std::vector<Rect>;

    Screen() noexcept;
    Screen(const Screen&) = default;
    Screen& operator=(const Screen&) = default;
    Screen(Screen&&) noexcept = default;
    Screen& operator=(Screen&&) noexcept = default;

    /**
     * Perform a flip of the buffers.
     *
     * This iterates the buffers and puts the composition buffer into the screen
     * buffers.
     *
     * @note This will call schedule_flip() automatically.
     */
    virtual void flip(const DamageArray& damage);

    /**
     * Schedule a flip to occur later.
     *
     * This is needed if a flip should occur sometime in the future to the
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
    EGT_NODISCARD Size size() const { return m_size; }

    /**
     * Bounding box for the screen.
     */
    EGT_NODISCARD Rect box() const { return Rect(Point(), m_size); }

    /**
     * Get the painter for the screen.
     **/
    EGT_NODISCARD Painter& painter() { return *m_painter; }

    /**
     * This function implements the algorithm for adding damage rectangles
     * to a list.
     *
     * @param[in,out] damage The starting and ending damage array.
     * @param[in] rect The new rectangle to add.
     *
     * @note This function may be implemented to be recursive.
     */
    static void damage_algorithm(Screen::DamageArray& damage, Rect rect);

    /**
     * Set if asynchronous buffer flips are used.
     */
    void async_flip(bool async)
    {
        m_async = async;
    }

    /**
     * Get the max brightness of the screen.
     *
     * @note Not all screens support this capability.
     */
    EGT_NODISCARD virtual size_t max_brightness() const;

    /**
     * Get the current brightness of the screen.
     *
     * @note Not all screens support this capability.
     */
    EGT_NODISCARD virtual size_t brightness() const;

    /**
     * Set the brightness of the screen.
     *
     * @param brightness Value from 0 to max_brightness().
     *
     * @note Not all screens support this capability.
     */
    virtual void brightness(size_t brightness);

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

    /**
     * Get the format of the screen.
     */
    EGT_NODISCARD PixelFormat format() const { return m_format; };

    /**
     * Returns true if the screen supports planes.
     */
    EGT_NODISCARD virtual bool have_planes() const { return false; }

    /**
     * Returns true if this is a screen used with the Microchip Graphic
     * Composer.
     */
    EGT_NODISCARD virtual bool is_composer() const { return false; }

    virtual void flush() {}

    virtual ~Screen() noexcept = default;

protected:

    /**
     * @param info Array of detail::FrameBufferInfos.
     * @param count Size of info array.  Zero is allowed.
     * @param size Size of the framebuffers.  They must all be the same.
     * @param format Format of the framebuffers.
     */
    void init(const detail::FrameBufferInfo* info, uint32_t count, const Size& size,
              PixelFormat format = PixelFormat::argb8888);

    /// @see init()
    void init(const Size& size, PixelFormat format = PixelFormat::argb8888)
    {
        init(nullptr, 0, size, format);
    }

    /// @private
    struct ScreenBuffer
    {
        explicit ScreenBuffer(Surface&& s) noexcept
            : surface(std::move(s))
        {
            damage.reserve(10);
        }

        Surface surface;

        /**
         * Each rect that needs to be copied from the back buffer.
         */
        DamageArray damage;

        void add_damage(const Rect& rect)
        {
            Screen::damage_algorithm(damage, rect);
        }
    };

    /// Copy the framebuffer to the current composition buffer.
    virtual void copy_to_buffer(ScreenBuffer& buffer);

    /// Copy the framebuffer to the current composition buffer.
    void copy_to_buffer_software(ScreenBuffer& buffer);

    /// Composition surface.
    Surface m_surface;

    /// Composition painter.
    std::unique_ptr<Painter> m_painter;

    /// Type used for an array of ScreenBuffer objects.
    using BufferArray = std::vector<ScreenBuffer>;

    /// Screen buffer array.
    BufferArray m_buffers;

    /// Size of the screen.
    Size m_size;

    /// Perform flips asynchronously if supported
    bool m_async{false};

    /// Format of the screen.
    PixelFormat m_format{};
};

}
}

#endif
