/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <egt/window.h>

#include <gst/gst.h>

#include <string>

namespace egt
{
inline namespace v1
{

class CameraWindow : public Window
{
public:
    CameraWindow(const Size& size,
                 const std::string& device = "/dev/video0",
                 pixel_format format = pixel_format::xrgb8888,
                 windowhint hint = windowhint::overlay);

    CameraWindow(const Rect& rect,
                 const std::string& device = "/dev/video0",
                 pixel_format format = pixel_format::xrgb8888,
                 windowhint hint = windowhint::overlay);

    virtual void top_draw() override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual ~CameraWindow();

    bool start();

    void stop();

    void move(const Point& p);

    float scale();

    void set_scale(float value);

    std::string get_error_message();

protected:
    std::string m_devnode;
    int m_height;
    GstElement* m_pipeline;
    GstElement* m_appsink;
    GstSample* m_camerasample;
    std::string m_err_message;

    void createImpl(const Size& size);

    void copy_frame(GstSample* sample);

    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);

    static GstFlowReturn on_new_buffer(GstElement* elt, gpointer data);

private:
    CameraWindow() = delete;
};

} //namespace v1

} //namespace egt
