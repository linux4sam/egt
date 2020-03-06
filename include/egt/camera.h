/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_CAMERA_H
#define EGT_CAMERA_H

#include <egt/detail/meta.h>
#include <egt/window.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

namespace detail
{
class CameraImpl;
}

/**
 * A CameraWindow is a widget to capture image feed from the camera
 * sensor and render it on screen using gstreamer media framework.
 *
 * It has a bounding rectangle, device, format and WindowHint. These
 * properties can be manipulated to create a camera window either as
 * a basic window or an overlay plane.
 *
 */
class EGT_API CameraWindow : public Window
{
public:

    /**
     * Event signal
     * @{
     */
    /**
     * Generated when an error occurs.
     */
    detail::Signal<const std::string&> on_error;
    /** @} */

    /**
     * Create a camera window.
     *
     * @param[in] device Camera device node.
     * @param[in] format_hint Requested format of the Window. This only applies
     *            if this Window will be responsible for creating a backing
     *            screen. Otherwise, the Window will use whatever format the
     *            existing screen has. This is only a hint.
     * @param[in] hint Requested Window type. This only applies if this Window
     *            will be responsible for creating a backing screen.  This is
     *            only a hint.
     *
     * @note Only WindowHint::heo_overlay can use yuyv, nv21 and yuv420 pixel
     * formats.
     */
    explicit CameraWindow(const std::string& device = "/dev/video0",
                          PixelFormat format_hint = PixelFormat::yuyv,
                          WindowHint hint = WindowHint::overlay);

    /**
     * Create a camera window.
     *
     * @param[in] rect Initial rectangle of the Window.
     * @param[in] device Camera device node.
     * @param[in] format_hint Requested format of the Window. This only applies
     *            if this Window will be responsible for creating a backing
     *            screen. Otherwise, the Window will use whatever format the
     *            existing screen has. This is only a hint.
     * @param[in] hint Requested Window type. This only applies if this Window
     *            will be responsible for creating a backing screen.  This is
     *            only a hint.
     *
     * @note Only WindowHint::heo_overlay can use yuyv, nv21 and yuv420 pixel
     * formats.
     */
    explicit CameraWindow(const Rect& rect,
                          const std::string& device = "/dev/video0",
                          PixelFormat format_hint = PixelFormat::yuyv,
                          WindowHint hint = WindowHint::overlay);

    virtual void do_draw() override
    {
        // video windows don't draw
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Initialize camera pipeline to capture image feed from the camera
     * sensor and render to Window.
     *
     * @return true on success
     */
    virtual bool start();

    /**
     * Stop camera.
     */
    virtual void stop();

    virtual void scale(float scalex, float scaley) override;

    virtual ~CameraWindow();

protected:
    float m_scalex{1.0};
    float m_scaley{1.0};

    std::unique_ptr<detail::CameraImpl> m_camera_impl;

};

} //namespace v1

} //namespace egt

#endif
