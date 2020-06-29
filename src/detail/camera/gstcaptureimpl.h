/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_CAMERA_GSTCAPTUREIMPL_H
#define EGT_SRC_DETAIL_CAMERA_GSTCAPTUREIMPL_H

#include "egt/capture.h"
#include <condition_variable>
#include <gst/gst.h>
#include <mutex>
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
                PixelFormat format,
                experimental::CameraCapture::ContainerType container,
                const std::string& device);

    // special functions deleted because they are never used
    CaptureImpl(const CaptureImpl&) = delete;
    CaptureImpl& operator=(const CaptureImpl&) = delete;
    CaptureImpl(CaptureImpl&&) = delete;
    CaptureImpl& operator=(CaptureImpl&&) = delete;

    void set_output(const std::string& output,
                    experimental::CameraCapture::ContainerType container = experimental::CameraCapture::ContainerType::avi,
                    PixelFormat format = PixelFormat::yuv420);

    bool start();

    void stop();

    ~CaptureImpl() noexcept;

protected:
    experimental::CameraCapture& m_interface;
    std::string m_output;
    PixelFormat m_format;
    experimental::CameraCapture::ContainerType m_container;
    std::string m_devnode;
    GstElement* m_pipeline{nullptr};
    GstSample* m_camerasample{nullptr};
    GMainLoop* m_gmainLoop{nullptr};
    std::thread m_gmainThread;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    static GstFlowReturn on_new_buffer(GstElement* elt, gpointer data);
    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);
    void get_camera_device_caps();
};

}

}

}

#endif
