/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/egtlog.h"
#include "detail/multimedia/gstdecoderimpl.h"
#include "detail/multimedia/gstcaptureimpl.h"
#include "detail/multimedia/gstmeta.h"
#include "egt/app.h"
#include "egt/detail/meta.h"
#include "egt/types.h"
#include <exception>
#include <gst/gst.h>
#include <sstream>

namespace egt
{
inline namespace v1
{
namespace detail
{

CaptureImpl::CaptureImpl(experimental::CameraCapture& iface,
                         // NOLINTNEXTLINE(modernize-pass-by-value)
                         const std::string& output,
                         PixelFormat format,
                         experimental::CameraCapture::ContainerType container,
                         // NOLINTNEXTLINE(modernize-pass-by-value)
                         const std::string& device)
    : m_interface(iface),
      m_output(output),
      m_format(format),
      m_container(container),
      m_devnode(device)
{
    detail::gstreamer_init();

    m_gmainLoop = g_main_loop_new(nullptr, FALSE);
    m_gmainThread = std::thread(g_main_loop_run, m_gmainLoop);
}

gboolean CaptureImpl::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    auto impl = static_cast<CaptureImpl*>(data);

    std::unique_lock<std::mutex> lock(impl->m_mutex);

    EGTLOG_TRACE("gst message: {}", GST_MESSAGE_TYPE_NAME(message));

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gstreamer_message_parse(gst_message_parse_error, message, error, debug);
        if (error)
        {
            EGTLOG_DEBUG("gst error: {} {}",
                         error->message,
                         debug ? debug.get() : "");

            if (Application::check_instance())
            {
                asio::post(Application::instance().event().io(), [impl, error = std::move(error)]()
                {
                    impl->m_interface.on_error.invoke(error->message);
                });
            }
        }
        break;
    }
    case GST_MESSAGE_WARNING:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gstreamer_message_parse(gst_message_parse_warning, message, error, debug);
        if (error)
        {
            EGTLOG_DEBUG("gst warning: {} {}",
                         error->message,
                         debug ? debug.get() : "");
        }
        break;
    }
    case GST_MESSAGE_INFO:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gstreamer_message_parse(gst_message_parse_info, message, error, debug);
        if (error)
        {
            EGTLOG_DEBUG("gst info: {} {}",
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

        std::string devnode = gstreamer_get_device_path(device);

        if (Application::check_instance())
        {
            asio::post(Application::instance().event().io(), [impl, devnode]()
            {
                impl->m_interface.on_connect.invoke(devnode);
            });
        }

        break;
    }
    case GST_MESSAGE_DEVICE_REMOVED:
    {
        GstDevice* device;
        gst_message_parse_device_removed(message, &device);

        std::string devnode = gstreamer_get_device_path(device);

        impl->m_condition.notify_one();

        if (Application::check_instance())
        {
            asio::post(Application::instance().event().io(), [impl, devnode]()
            {
                impl->m_interface.on_disconnect.invoke(devnode);
            });
        }
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
                             experimental::CameraCapture::ContainerType container,
                             PixelFormat format)
{
    m_output = output;
    m_format = format;
    m_container = container;
}

void CaptureImpl::get_camera_device_caps()
{
    std::tuple<std::string, std::string, std::string,
        std::vector<std::tuple<int, int>>> caps = detail::get_camera_device_caps(m_devnode);

    m_devnode = std::get<0>(caps);
}

bool CaptureImpl::start()
{
    std::string pipe;

    get_camera_device_caps();

    switch (m_container)
    {
    case experimental::CameraCapture::ContainerType::avi:
    {
        static constexpr auto avi_pipe =
            "v4l2src device={} ! videoconvert ! video/x-raw,width={},height={},format={},framerate=30/1 ! " \
            "avimux ! filesink location={}";

        const auto format = detail::gstreamer_format(m_format);
        pipe = fmt::format(avi_pipe, m_devnode, 320, 240, format, m_output);
        break;
    }
    case experimental::CameraCapture::ContainerType::mpeg2ts:
    {
        static constexpr auto mpeg2ts_pipe =
            "v4l2src device={} ! videoconvert ! video/x-raw,width={},height={},format={},framerate=30/1 ! " \
            "avenc_mpeg2video ! mpegtsmux ! filesink location={}";

        const auto format = detail::gstreamer_format(m_format);
        pipe = fmt::format(mpeg2ts_pipe, m_devnode, 320, 240, format, m_output);

        break;
    }
    }

    EGTLOG_DEBUG(pipe);

    /* Make sure we don't leave orphan references */
    stop();

    GError* error = nullptr;
    m_pipeline = gst_parse_launch(pipe.c_str(), &error);
    if (!m_pipeline)
    {
        m_interface.on_error.invoke(fmt::format("failed to create pipeline: {}", error->message));
        return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(bus, &bus_callback, this);
    gst_object_unref(bus);

    int ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        m_interface.on_error.invoke("failed to set pipeline to play state");
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
            detail::error("set pipeline to NULL state failed");
        }

        g_object_unref(m_pipeline);
        m_pipeline = nullptr;
    }
}

CaptureImpl::~CaptureImpl() noexcept
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
