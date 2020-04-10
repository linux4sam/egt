/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SCREEN_KMSSCREEN_H
#define EGT_DETAIL_SCREEN_KMSSCREEN_H

/**
 * @file
 * @brief Working with KMS screens.
 */

#include <egt/detail/meta.h>
#include <egt/detail/screen/kmstype.h>
#include <egt/geometry.h>
#include <egt/screen.h>
#include <egt/window.h>
#include <memory>
#include <vector>

struct plane_data;
struct kms_device;

namespace egt
{
inline namespace v1
{
namespace detail
{
struct planeid;
class KMSOverlay;
struct FlipThread;

/**
 * Screen in an KMS dumb buffer.
 *
 * This uses libplanes to modeset and configure planes.
 */
class EGT_API KMSScreen : public Screen
{
public:

    /**
     * @param allocate_primary_plane Allocate a primary plane, or create a trash
     *        buffer instead.
     * @param format Requested format for the screen.
     */
    explicit KMSScreen(bool allocate_primary_plane = true,
                       PixelFormat format = PixelFormat::rgb565);

    EGT_OPS_NOCOPY_MOVE(KMSScreen);

    ~KMSScreen() noexcept override;

    /// Available plane types.
    enum class plane_type
    {
        overlay,
        primary,
        cursor
    };

    /**
     * Count the number of available hardware planes with a specific type.
     */
    uint32_t count_planes(plane_type type = plane_type::overlay);

    /// Get a pointer to the KMSScreen instance.
    static KMSScreen* instance();

    void schedule_flip() override;

    uint32_t index() override;

    /// Close and release the screen.
    void close();

    EGT_NODISCARD size_t max_brightness() const override;

    EGT_NODISCARD size_t brightness() const override;

    void brightness(size_t brightness) override;

    /// Get the number of buffers to use for KMS planes.
    static uint32_t max_buffers();

    /// Allocate an overlay plane.
    unique_plane_t allocate_overlay(const Size& size,
                                    PixelFormat format = PixelFormat::argb8888,
                                    WindowHint hint = WindowHint::automatic);

    /// Deallocate an overlay plane.
    void deallocate_overlay(plane_data* plane);

protected:
    /// Allocate an overlay plane.
    plane_data* overlay_plane_create(const Size& size,
                                     PixelFormat format,
                                     plane_type type);

    /// Internal DRM/KMS file descriptor.
    int m_fd{-1};
    /// Instance of the KMS device.
    struct kms_device* m_device {nullptr};
    /// Plane instance pointer.
    unique_plane_t m_plane;
    /// Current flip index.
    uint32_t m_index{0};
    /// Global array used to keep track of allocated planes
    static std::vector<planeid> m_used;
    /// Internal thread pool for flipping.
    std::unique_ptr<FlipThread> m_pool;
    /// Enable GFX2D
    bool m_gfx2d {false};

    friend class detail::KMSOverlay;
};

}
}
}

#endif
