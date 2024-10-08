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
class GstDecoderImpl;
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
protected:

    /// @private
    std::unique_ptr<detail::GstDecoderImpl> m_camera_impl;

public:

    /**
     * Event signal
     * @{
     */
    /**
     * Generated when an error occurs.
     */
    SignalW<const std::string&> on_error;

    /**
     * Generated when an USB camera connected.
     */
    SignalW<const std::string&> on_connect;

    /**
     * Generated when an USB camera disconnected.
     */
    SignalW<const std::string&> on_disconnect;
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
    CameraWindow(Serializer::Properties& props)
        : CameraWindow(props, false)
    {
    }

protected:

    CameraWindow(Serializer::Properties& props, bool is_derived);

public:

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

    /**
     * Set the camera pipeline to the play state. In other words, play the
     * video from the camera.
     */
    bool play();

    /**
     * Check is the camera pipeline in play state.
     *
     * @return true on success
     */
    EGT_NODISCARD bool playing() const;

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

    void serialize(Serializer& serializer) const override;

    ~CameraWindow() noexcept override;

private:

    void deserialize(Serializer::Properties& props);

};

}
}

#endif
