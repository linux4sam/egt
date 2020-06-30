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
    CameraImpl(CameraWindow& interface, const Rect& rect,
               const std::string& device);

    virtual void draw(Painter& painter, const Rect& rect);

    virtual bool start();

    virtual void stop();

    virtual void scale(float scalex, float scaley);

    virtual ~CameraImpl();

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
get_camera_device_caps(const std::string& dev_name, BusCallback bus_callback, void* instance);

} //End of detail

} //End of v1

} //End of egt


#endif
