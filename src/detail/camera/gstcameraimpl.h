/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_CAMERA_GSTCAMERAIMPL_H
#define EGT_SRC_DETAIL_CAMERA_GSTCAMERAIMPL_H

#include "egt/camera.h"
#include <gst/gst.h>
#include <string>
#include <thread>

namespace egt
{
inline namespace v1
{
namespace detail
{

class CameraImpl
{
public:
    CameraImpl(CameraWindow& iface, const Rect& rect,
               const std::string& device);

    // special functions deleted because they are never used
    CameraImpl(const CameraImpl&) = delete;
    CameraImpl& operator=(const CameraImpl&) = delete;
    CameraImpl(CameraImpl&&) = delete;
    CameraImpl& operator=(CameraImpl&&) = delete;

    void draw(Painter& painter, const Rect& rect);

    bool start();

    void stop();

    void device(const std::string& device);

    std::string device() const;

    void scale(float scalex, float scaley);

    std::vector<std::string> list_devices();

    ~CameraImpl() noexcept;

protected:
    CameraWindow& m_interface;
    std::string m_devnode;
    GstElement* m_pipeline{nullptr};
    GstElement* m_appsink{nullptr};
    GstSample* m_camerasample{nullptr};
    Rect m_rect;
    GMainLoop* m_gmain_loop{nullptr};
    std::thread m_gmain_thread;
    std::string m_caps_name;
    std::string m_caps_format;
    std::vector<std::tuple<int, int>> m_resolutions;

    void get_camera_device_caps();

    static GstFlowReturn on_new_buffer(GstElement* elt, gpointer data);

    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);
};

using BusCallback = gboolean(*)(GstBus* bus, GstMessage* message, gpointer data);

std::tuple<std::string, std::string, std::string, std::vector<std::tuple<int, int>>>
get_camera_device_caps(const std::string& dev_name);

std::vector<std::string> get_camera_device_list();

} //End of detail

} //End of v1

} //End of egt


#endif
