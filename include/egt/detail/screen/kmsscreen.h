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

#include <egt/geometry.h>
#include <egt/detail/screen/kmsoverlay.h>
#include <egt/screen.h>
#include <egt/window.h>
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

/**
 * Screen in an KMS dumb buffer.
 *
 * This uses libplanes to modeset and configure planes.
 */
class KMSScreen : public Screen
{
public:

    explicit KMSScreen(bool primary = true);

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

    struct plane_data* allocate_overlay(const Size& size,
                                        pixel_format format = pixel_format::argb8888,
                                        windowhint hint = windowhint::automatic);

    virtual ~KMSScreen();

    /**
     * Get the number of buffers to use for KMS planes.
     */
    static uint32_t max_buffers();

protected:

    int m_fd{-1};
    struct kms_device* m_device {nullptr};
    struct plane_data* m_plane {nullptr};
    uint32_t m_index{0};
    static std::vector<planeid> m_used;

    friend class detail::KMSOverlay;
};

}
}
}

#endif
