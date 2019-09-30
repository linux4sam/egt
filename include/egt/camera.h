/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_CAMERA_H
#define EGT_CAMERA_H

#include <egt/window.h>

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
 * It has a bounding rectangle, device, format and windowhint. These
 * properties can be manipulated to create a camera window either as
 * a basic window or an overlay plane.
 *
 */
class CameraWindow : public Window
{
public:

    /**
     * Create a camera window.
     *
     * @param device is a cameras device node.
     * @param format is a pixel format of a window or an overlay plane.
     * @param hint used for configuring window backend's.
     *
     * @note Only windowhint::heo_overlay can use yuyv, nv21 and yuv420 pixel
     * formats.
     */
    explicit CameraWindow(const std::string& device = "/dev/video0",
                          pixel_format format = pixel_format::xrgb8888,
                          windowhint hint = windowhint::overlay);

    /**
     * Create a camera window.
     *
     * @param rect is a size of window with offset x & y.
     * @param device is a cameras device node.
     * @param format is a pixel format of a window or an overlay plane.
     * @param hint used for configuring window backend's.
     *
     * @note Only windowhint::heo_overlay can use yuyv, nv21 and yuv420 pixel
     * formats.
     */
    explicit CameraWindow(const Rect& rect,
                          const std::string& device = "/dev/video0",
                          pixel_format format = pixel_format::xrgb8888,
                          windowhint hint = windowhint::overlay);

    virtual void do_draw() override
    {
        // video windows don't draw
    }

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Initialize camera pipeline to capture image feed from the camera
     * sensor and render to screen.
     *
     * @return true on success
     */
    bool start();

    /**
     * Stop camera.
     */
    void stop();

    /**
     * Get error message.
     */
    std::string error_message() const;

    virtual ~CameraWindow() = default;

protected:
    std::shared_ptr<detail::CameraImpl> m_cameraImpl;

};

} //namespace v1

} //namespace egt

#endif
