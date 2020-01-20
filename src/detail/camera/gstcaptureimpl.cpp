/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/camera/gstcameraimpl.h"
#include "detail/camera/gstcaptureimpl.h"
#include "detail/video/gstmeta.h"
#include "egt/app.h"
#include "egt/detail/meta.h"
#include "egt/types.h"
#include <exception>
#include <gst/gst.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>

namespace egt
{
inline namespace v1
{
namespace detail
{

CaptureImpl::CaptureImpl(experimental::CameraCapture& interface,
                         const std::string& output,
                         PixelFormat format,
                         experimental::CameraCapture::container_type container,
                         const std::string& device)
    : m_interface(interface),
      m_output(output),
      m_format(format),
      m_container(container),
      m_devnode(device)
{
    GError* err = nullptr;
    if (!gst_init_check(nullptr, nullptr, &err))
    {
        std::ostringstream ss;
        ss << "failed to initialize gstreamer: ";
        if (err && err->message)
        {
            ss << err->message;
            g_error_free(err);
        }
        else
        {
            ss << "unknown error";
        }

        throw std::runtime_error(ss.str());
    }

    /**
     * check for cache file by finding a playback plugin.
     * if gst_registry_find_plugin returns NULL, then no
     * cache file present and assume GSTREAMER1_PLUGIN_REGISTRY
     * is disabled in gstreamer package.
     */
    if (!gst_registry_find_plugin(gst_registry_get(), "playback"))
    {
        SPDLOG_DEBUG("manually loading gstreamer plugins");

        auto plugins =
        {
            "/usr/lib/gstreamer-1.0/libgstcoreelements.so",
            "/usr/lib/gstreamer-1.0/libgsttypefindfunctions.so",
            "/usr/lib/gstreamer-1.0/libgstplayback.so",
            "/usr/lib/gstreamer-1.0/libgstapp.so",
            "/usr/lib/gstreamer-1.0/libgstvideo4linux2.so",
            "/usr/lib/gstreamer-1.0/libgstvideoscale.so",
            "/usr/lib/gstreamer-1.0/libgstvideoconvert.so",
            "/usr/lib/gstreamer-1.0/libgstavi.so",
            "/usr/lib/gstreamer-1.0/libgstmpegtsmux.so",
            "/usr/lib/gstreamer-1.0/libgstlibav.so",
            "/usr/lib/gstreamer-1.0/libgstvideoparsersbad.so",
        };

        for (auto& plugin : plugins)
        {
            GError* error = nullptr;
            gst_plugin_load_file(plugin, &error);
            if (error)
            {
                if (error->message)
                    spdlog::error("load plugin error: {}", error->message);
                g_error_free(error);
            }
        }
    }

    m_gmainLoop = g_main_loop_new(nullptr, FALSE);
    m_gmainThread = std::thread(g_main_loop_run, m_gmainLoop);
}

gboolean CaptureImpl::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    auto impl = reinterpret_cast<CaptureImpl*>(data);

    std::unique_lock<std::mutex> lock(impl->m_mutex);

    SPDLOG_TRACE("gst message: {}", GST_MESSAGE_TYPE_NAME(message));

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gst_message_parse(gst_message_parse_error, message, error, debug);
        if (error)
        {
            SPDLOG_DEBUG("gst error: {} {}",
                         error->message,
                         debug ? debug.get() : "");

            asio::post(Application::instance().event().io(), [impl, error = std::move(error)]()
            {
                impl->m_err_message = error->message;
                impl->m_interface.on_error.invoke();
            });
        }
        break;
    }
    case GST_MESSAGE_WARNING:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gst_message_parse(gst_message_parse_warning, message, error, debug);
        if (error)
        {
            SPDLOG_DEBUG("gst warning: {} {}",
                         error->message,
                         debug ? debug.get() : "");
        }
        break;
    }
    case GST_MESSAGE_INFO:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gst_message_parse(gst_message_parse_info, message, error, debug);
        if (error)
        {
            SPDLOG_DEBUG("gst info: {} {}",
                         error->message,
                         debug ? debug.get() : "");
        }
        break;
    }
    case GST_MESSAGE_EOS:
    {
        impl->m_condition.notify_one();
        break;
    }
    case GST_MESSAGE_DEVICE_ADDED:
    {
        GstDevice* device;
        gst_message_parse_device_added(message, &device);
        GstStringHandle name{gst_device_get_display_name(device)};
        SPDLOG_DEBUG("device added: {}", name.get());
        gst_object_unref(device);

        asio::post(Application::instance().event().io(), [impl]()
        {
            if (impl->start())
            {
                impl->m_err_message = "";
                impl->m_interface.on_error.invoke();
            }
        });

        break;
    }
    case GST_MESSAGE_DEVICE_REMOVED:
    {
        GstDevice* device;
        gst_message_parse_device_removed(message, &device);
        GstStringHandle name{gst_device_get_display_name(device)};
        SPDLOG_DEBUG("device removed: {}", name.get());
        gst_object_unref(device);

        impl->m_condition.notify_one();

        impl->stop();

        asio::post(Application::instance().event().io(), [impl, name = std::move(name)]()
        {
            impl->m_err_message = "device removed: " + std::string(name.get());
            impl->m_interface.on_error.invoke();
        });
        break;
    }
    default:
        break;
    }

    /* we want to be notified again if there is a message on the bus, so
     * returning true (false means we want to stop watching for messages
     * on the bus and our callback should not be called again)
     */
    return true;
}

void CaptureImpl::set_output(const std::string& output,
                             experimental::CameraCapture::container_type container,
                             PixelFormat format)
{
    m_output = output;
    m_format = format;
    m_container = container;
}

void CaptureImpl::get_camera_device_caps()
{
    m_devnode = detail::get_camera_device_caps(&bus_callback, this);
}

bool CaptureImpl::start()
{
    std::string pipe;

    get_camera_device_caps();

    switch (m_container)
    {
    case experimental::CameraCapture::container_type::avi:
    {
        static const auto avi_pipe =
            "v4l2src device={} ! videoconvert ! video/x-raw,width={},height={},format={},framerate=30/1 ! " \
            "avimux ! filesink location={}";

        const auto format = detail::gstreamer_format(m_format);
        pipe = fmt::format(avi_pipe, m_devnode, 320, 240, format, m_output);
        break;
    }
    case experimental::CameraCapture::container_type::mpeg2ts:
    {
        static const auto mpeg2ts_pipe =
            "v4l2src device={} ! videoconvert ! video/x-raw,width={},height={},format={},framerate=30/1 ! " \
            "avenc_mpeg2video ! mpegtsmux ! filesink location={}";

        const auto format = detail::gstreamer_format(m_format);
        pipe = fmt::format(mpeg2ts_pipe, m_devnode, 320, 240, format, m_output);

        break;
    }
    }

    SPDLOG_DEBUG(pipe);

    /* Make sure we don't leave orphan references */
    stop();

    GError* error = nullptr;
    m_pipeline = gst_parse_launch(pipe.c_str(), &error);
    if (!m_pipeline)
    {
        m_err_message = error->message;
        spdlog::error("failed to create pipeline: {}", m_err_message);
        return false;
    }

    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(bus, &bus_callback, this);
    gst_object_unref(bus);

    int ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        m_err_message = "failed to set pipeline to play state";
        spdlog::error(m_err_message);
        stop();
        return false;
    }
    return true;
}

void CaptureImpl::stop()
{
    if (m_pipeline)
    {
        bool wait = GST_STATE(m_pipeline) == GST_STATE_PLAYING;

        std::unique_lock<std::mutex> lock(m_mutex);
        if (wait)
        {
            gst_element_send_event(m_pipeline, gst_event_new_eos());
            m_condition.wait(lock);
        }

        GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == ret)
        {
            m_err_message = "set pipeline to NULL state failed";
            spdlog::error(m_err_message);
        }

        g_object_unref(m_pipeline);
        m_pipeline = nullptr;
    }
}

std::string CaptureImpl::error_message() const
{
    return m_err_message;
}

CaptureImpl::~CaptureImpl()
{
    stop();

    if (m_gmainLoop)
    {
        /*
         * check loop is running to avoid race condition when stop is called too early
         */
        if (g_main_loop_is_running(m_gmainLoop))
        {
            //stop loop and wait
            g_main_loop_quit(m_gmainLoop);
        }
        m_gmainThread.join();
        g_main_loop_unref(m_gmainLoop);
    }
}

}
}
}
