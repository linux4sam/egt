/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_CAMERA_GSTCAPTUREIMPL_H
#define EGT_DETAIL_CAMERA_GSTCAPTUREIMPL_H

#include "egt/capture.h"
#include <gst/gst.h>
#include <string>
#include <thread>

namespace egt
{
inline namespace v1
{
namespace detail
{

class CaptureImpl
{
public:
    CaptureImpl(experimental::CameraCapture& interface,
                const std::string& output,
                pixel_format format,
                experimental::CameraCapture::container_type container,
                const std::string& device);

    virtual void set_output(const std::string& output,
                            experimental::CameraCapture::container_type container = experimental::CameraCapture::container_type::avi,
                            pixel_format format = pixel_format::yuv420);

    virtual bool start();

    virtual void stop();

    std::string error_message() const;

    virtual ~CaptureImpl();

protected:
    experimental::CameraCapture& m_interface;
    std::string m_output;
    pixel_format m_format;
    experimental::CameraCapture::container_type m_container;
    std::string m_devnode;
    GstElement* m_pipeline{nullptr};
    GstSample* m_camerasample{nullptr};
    std::string m_err_message;
    GMainLoop* m_gmainLoop{nullptr};
    std::thread m_gmainThread;

    static GstFlowReturn on_new_buffer(GstElement* elt, gpointer data);
    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);
};

}

}

}

#endif
