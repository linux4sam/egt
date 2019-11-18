/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/camera/gstcaptureimpl.h"
#include "egt/app.h"
#include "egt/detail/meta.h"
#include <exception>
#include <gst/gst.h>
#include <map>
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
                         pixel_format format,
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

    m_gmainLoop = g_main_loop_new(nullptr, FALSE);
    m_gmainThread = std::thread(g_main_loop_run, m_gmainLoop);
}

gboolean CaptureImpl::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    auto impl = reinterpret_cast<CaptureImpl*>(data);

    std::unique_lock<std::mutex> lock(impl->m_mutex);

    SPDLOG_DEBUG("gst message: {}", GST_MESSAGE_TYPE_NAME(message));

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        GError* error;
        gchar* debug;
        gst_message_parse_error(message, &error, &debug);
        std::string error_message = error->message;
        SPDLOG_DEBUG("GST_MESSAGE_ERROR from element {} {}",
                     GST_OBJECT_NAME(message->src), error->message);
        SPDLOG_DEBUG("GST_MESSAGE_ERROR Debugging info: {}",
                     (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);

        asio::post(Application::instance().event().io(), [impl, error_message]()
        {
            impl->m_err_message = error_message;
            Event event(eventid::event2);
            impl->m_interface.invoke_handlers(event);
        });
        break;
    }
    case GST_MESSAGE_WARNING:
    {
        GError* error;
        gchar* debug;
        gst_message_parse_warning(message, &error, &debug);
        SPDLOG_DEBUG("GST_MESSAGE_WARNING from element {} {}", GST_OBJECT_NAME(message->src), error->message);
        SPDLOG_DEBUG("GST_MESSAGE_WARNING Debugging info: {}", (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);
        break;
    }
    case GST_MESSAGE_INFO:
    {
        GError* error;
        gchar* debug;
        gchar* name = gst_object_get_path_string(GST_MESSAGE_SRC(message));
        gst_message_parse_info(message, &error, &debug);
        SPDLOG_DEBUG("GST_MESSAGE_INFO: {} ", error->message);
        if (debug)
        {
            SPDLOG_DEBUG("GST_MESSAGE_INFO: {}", debug);
        }
        g_clear_error(&error);
        g_free(debug);
        g_free(name);
        break;
    }
    case GST_MESSAGE_EOS:
    {
        impl->m_condition.notify_one();
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
                             pixel_format format)
{
    m_output = output;
    m_format = format;
    m_container = container;
}

static inline std::string gstreamer_format(pixel_format format)
{
    static const std::map<pixel_format, std::string> formats =
    {
        {pixel_format::argb8888, "BGRx"},
        {pixel_format::xrgb8888, "BGRx"},
        {pixel_format::rgb565, "RGB16"},
        {pixel_format::yuv420, "I420"},
        {pixel_format::yuyv, "YUY2"},
        {pixel_format::nv21, "NV21"},
    };

    const auto i = formats.find(format);
    if (i != formats.end())
        return i->second;

    return {};
}

bool CaptureImpl::start()
{
    std::string pipe;

    switch (m_container)
    {
    case experimental::CameraCapture::container_type::avi:
    {
        static const auto avi_pipe =
            "v4l2src device={} ! videoconvert ! video/x-raw,width={},height={},format={},framerate=30/1 ! " \
            "avimux ! filesink location={}";

        const auto format = gstreamer_format(m_format);
        pipe = fmt::format(avi_pipe, m_devnode, 320, 240, format, m_output);
        break;
    }
    case experimental::CameraCapture::container_type::mpeg2ts:
    {
        static const auto mpeg2ts_pipe =
            "v4l2src device={} ! videoconvert ! video/x-raw,width={},height={},format={},framerate=30/1 ! " \
            "avenc_mpeg2video ! mpegtsmux ! filesink location={}";

        const auto format = gstreamer_format(m_format);
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
