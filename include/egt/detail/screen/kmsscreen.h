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
class KMSOverlay;
struct planeid;
class FlipThread;

/**
 * Screen in an KMS dumb buffer.
 *
 * This uses libplanes to modeset and configure planes.
 */
class EGT_API KMSScreen : public Screen
{
public:

    explicit KMSScreen(bool allocate_primary_plane = true,
                       PixelFormat format = PixelFormat::rgb565);

    enum class plane_type
    {
        overlay,
        primary,
        cursor
    };

    /**
     * Count the number of planes with a specific type.
     */
    uint32_t count_planes(plane_type type = plane_type::overlay);

    static KMSScreen* instance();

    void schedule_flip() override;

    uint32_t index() override;

    void close();

    unique_plane_t allocate_overlay(const Size& size,
                                    PixelFormat format = PixelFormat::argb8888,
                                    WindowHint hint = WindowHint::automatic);

    void deallocate_overlay(plane_data* plane);

    virtual size_t max_brightness() const override;

    virtual size_t brightness() const override;

    virtual void brightness(size_t brightness) override;

    virtual ~KMSScreen();

    /**
     * Get the number of buffers to use for KMS planes.
     */
    static uint32_t max_buffers();

protected:

    plane_data* overlay_plane_create(const Size& size,
                                     PixelFormat format,
                                     plane_type type);

    int m_fd{-1};
    struct kms_device* m_device {nullptr};
    unique_plane_t m_plane;
    uint32_t m_index{0};
    static std::vector<planeid> m_used;
    std::unique_ptr<FlipThread> m_pool;
    bool m_gfx2d {false};

    friend class detail::KMSOverlay;
};

}
}
}

#endif
