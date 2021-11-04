/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_CAMERA_H
#define EGT_CAMERA_H

/**
 * @file
 * @brief Camera window support.
 */

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
    Signal<const std::string&> on_error;

    /**
     * Generated when an USB camera connected.
     */
    Signal<const std::string&> on_connect;

    /**
     * Generated when an USB camera disconnected.
     */
    Signal<const std::string&> on_disconnect;
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
     * @param[in] rect Initial rectangle of the widget.
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

    /**
     * Construct a camera window.
     *
     * @param[in] props list of widget argument and its properties.
     */
    CameraWindow(Serializer::Properties& props);

    CameraWindow(const CameraWindow&) = delete;
    CameraWindow& operator=(const CameraWindow&) = delete;
    CameraWindow(CameraWindow&&) = default;
    CameraWindow& operator=(CameraWindow&&) = default;

    void do_draw() override
    {
        // video windows don't draw
    }

    void draw(Painter& painter, const Rect& rect) override;

    /**
     * Initialize camera pipeline to capture image feed from the camera
     * sensor and render to Window.
     *
     * @return true on success
     */
    bool start();

    /*
     * set camera device node.
     *
     *  @param[in] device Camera device node.
     */
    void device(const std::string& device);

    /*
     * Get camera device node in use.
     */
    std::string device() const;

    /*
     * Get the list of camera devices
     */
    std::vector<std::string> list_devices();

    /**
     * Stop camera.
     */
    void stop();

    using Window::scale;

    void scale(float hscale, float vscale) override;

    /**
     * Get horizontal scale value.
     */
    EGT_NODISCARD float hscale() const
    {
        return m_hscale;
    }

    /**
     * Get vertical scale value.
     */
    EGT_NODISCARD float vscale() const
    {
        return m_vscale;
    }

    void serialize(Serializer& serializer) const override;

    ~CameraWindow() noexcept override;

protected:

    /// Horizontal scale value.
    float m_hscale{1.0};

    /// Vertical scale value.
    float m_vscale{1.0};

    /// @private
    std::unique_ptr<detail::CameraImpl> m_camera_impl;

private:

    void deserialize(Serializer::Properties& props);

};

}
}

#endif
