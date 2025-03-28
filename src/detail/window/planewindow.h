/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_PLANEWINDOW_H
#define EGT_DETAIL_PLANEWINDOW_H

#include "detail/window/basicwindow.h"
#include <memory>

namespace egt
{
inline namespace v1
{
namespace detail
{
class KMSOverlay;

/**
 * A PlaneWindow backend uses a hardware overlay as a Screen.
 *
 * PlaneWindow separates "changing attributes" and "applying attributes".
 * This maps to the libplanes plane_apply() function. Which, is the same way
 * event handle() vs. draw() works in this toolkit. This is done as an
 * optimization to queue applying changes to a plane.
 */
class PlaneWindow : public BasicWindow
{
public:

    explicit PlaneWindow(Window* inter,
                         PixelFormat format = PixelFormat::argb8888,
                         WindowHint hint = WindowHint::automatic);

    PlaneWindow(const PlaneWindow&) = delete;
    PlaneWindow& operator=(const PlaneWindow&) = delete;
    PlaneWindow(PlaneWindow&&) noexcept = default;
    PlaneWindow& operator=(PlaneWindow&&) noexcept = default;

    void resize(const Size& size) override;

    void scale(float scalex, float scaley) override;

    void damage(const Rect& rect) override;

    void move(const Point& point) override;

    void begin_draw() override;

    void show() override;

    void hide() override;

    void allocate_screen() override;

    ~PlaneWindow() noexcept override;

protected:

    /**
     * The requested pixel format for the plane.
     */
    PixelFormat m_format{PixelFormat::argb8888};

    /**
     * The requested window hint for allocating the plane.
     */
    WindowHint m_hint{WindowHint::automatic};

    /**
     * When true, we have settings that need to be flushed to the plane
     * in begin_draw().
     */
    bool m_dirty{true};

    /**
     * Deallocate the underlying screen.
     */
    void deallocate_screen();

    /**
     * Instance of the KMS overlay.
     */
    std::unique_ptr<KMSOverlay> m_overlay;
};

}
}
}

#endif
