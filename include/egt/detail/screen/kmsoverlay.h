/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SCREEN_KMSOVERLAY_H
#define EGT_DETAIL_SCREEN_KMSOVERLAY_H

/**
 * @file
 * @brief Working with KMS screens.
 */

#include <egt/detail/meta.h>
#include <egt/detail/screen/kmstype.h>
#include <egt/input.h>
#include <egt/screen.h>
#include <egt/widgetflags.h>
#include <memory>

struct plane_data;

namespace egt
{
inline namespace v1
{
namespace detail
{
struct FlipThread;

/**
 * Screen in a KMS dumb buffer inside of an overlay plane.
 *
 * This uses libplanes to modeset and configure planes.
 */
class EGT_API KMSOverlay : public Screen
{
public:

    KMSOverlay() = delete;

    /**
     * @param size Size of the scree.
     * @param format Requested format for the screen.
     * @param hint Hint for the type of plane to create.
     */
    KMSOverlay(const Size& size, PixelFormat format, WindowHint hint);

    /**
     * Resize the hardware plane.
     *
     * @warning This is an expensive operation.  This will cause a re-allocation
     * of the plane and all buffers, destroying any existing buffers.
     */
    virtual void resize(const Size& size);
    /// Move the hardware plane to the specified position.
    virtual void position(const DisplayPoint& point);
    /**
     *  Change the hardware scale of the overlay plane.
     *
     * @param[in] hscale Horizontal scale, with 1.0 being 100%.
     * @param[in] vscale Vertical scale, with 1.0 being 100%.
     */
    virtual void scale(float hscale, float vscale);
    /**
     * Set the hardware pan size.
     */
    virtual void pan_size(const Size& size);
    /**
     * Set the hardware pan position in the plane buffer.
     */
    virtual void pan_pos(const Point& point);
    /// Get the horizontal scale value.
    virtual float hscale() const;
    /// Get the vertical scale value.
    virtual float vscale() const;

    /// Get internal DRM plane format.
    virtual uint32_t get_plane_format();

    /// Hide the plane from the display.
    virtual void hide();

    /// Show the plane on the display.
    virtual void show();

    /// Get the associated GEM handle for the overlay plane
    virtual int gem();

    /// Apply any pending changes to the hardware.
    virtual void apply();

    /// Get a raw pointer to the current framebuffer.
    void* raw();

    /// Get a pointer to the internal plane structure
    inline plane_data* s() const
    {
        return m_plane.get();
    }

    /// Rotate the hardware plane.
    virtual void rotate(uint32_t degrees);

    void schedule_flip() override;

    uint32_t index() override;

protected:
    /// Plane instance pointer.
    unique_plane_t m_plane;
    /// Current flip index.
    uint32_t m_index{0};
    /// Internal thread pool for flipping.
    std::unique_ptr<FlipThread> m_pool;
};

}
}
}

#endif
