/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/app.h"
#include "egt/camera.h"
#include <egt/detail/screen/kmsoverlay.h>
#include "egt/detail/screen/kmsscreen.h"
#include "egt/video.h"
#include <fstream>
#include <gst/gst.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <string>
#include <thread>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

class CameraImpl
{
public:
    explicit CameraImpl(CameraWindow& interface, const Size& size,
                        const std::string& device, bool useKmssink);

    explicit CameraImpl(CameraWindow& interface, const Rect& rect,
                        const std::string& device, bool useKmssink);

    void draw(Painter& painter, const Rect& rect);

    bool start();

    void stop();

    std::string get_error_message() const;

protected:
    CameraWindow& m_interface;
    std::string m_devnode;
    GstElement* m_pipeline{nullptr};
    GstElement* m_appsink{nullptr};
    GstSample* m_camerasample{nullptr};
    std::string m_err_message;
    Rect m_rect;
    bool m_usekmssink;

    static GstFlowReturn on_new_buffer(GstElement* elt, gpointer data);

    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);
};

CameraImpl::CameraImpl(CameraWindow& interface, const Size& size,
                       const std::string& device, bool useKmssink)
    : m_interface(interface),
      m_devnode(device),
      m_rect(Rect(size)),
      m_usekmssink(useKmssink)
{

}

CameraImpl::CameraImpl(CameraWindow& interface, const Rect& rect,
                       const std::string& device, bool useKmssink)
    : m_interface(interface),
      m_devnode(device),
      m_rect(rect),
      m_usekmssink(useKmssink)
{
}

gboolean CameraImpl::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    GError* error;
    gchar* debug;

    auto cameraImpl = reinterpret_cast<CameraImpl*>(data);

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        gst_message_parse_error(message, &error, &debug);
        cameraImpl->m_err_message = error->message;
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_ERROR from element {} {}",  GST_OBJECT_NAME(message->src), error->message);
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_ERROR Debugging info: {}", (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);

        asio::post(Application::instance().event().io(), [cameraImpl]()
        {
            Event event(eventid::event2);
            cameraImpl->m_interface.invoke_handlers(event);
        });
        break;
    }
    case GST_MESSAGE_WARNING:
    {
        gst_message_parse_warning(message, &error, &debug);
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_WARNING from element {} {}", GST_OBJECT_NAME(message->src), error->message);
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_WARNING Debugging info: {}", (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);
        break;
    }
    case GST_MESSAGE_INFO:
    {
        gchar* name = gst_object_get_path_string(GST_MESSAGE_SRC(message));
        gst_message_parse_info(message, &error, &debug);
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_INFO: {} ", error->message);
        if (debug)
        {
            SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_INFO: {}", debug);
        }
        g_clear_error(&error);
        g_free(debug);
        g_free(name);
        break;
    }
    case GST_MESSAGE_CLOCK_PROVIDE:
    {
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_CLOCK_PROVIDE");
        break;
    }
    case GST_MESSAGE_CLOCK_LOST:
    {
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_CLOCK_LOST");
        break;
    }
    case GST_MESSAGE_NEW_CLOCK:
    {
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_NEW_CLOCK");
        break;
    }
    case GST_MESSAGE_PROGRESS:
    {
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_PROGRESS");
        break;
    }
    case GST_MESSAGE_DURATION_CHANGED:
    {
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_DURATION_CHANGED");
        break;
    }
    case GST_MESSAGE_ELEMENT:
    {
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_ELEMENT");
        break;
    }
    case GST_MESSAGE_TAG:
    {
        SPDLOG_DEBUG("CameraWindow: GST_MESSAGE_TAG");
        break;
    }
    default:
    {
        SPDLOG_DEBUG("CameraWindow: default Message {}", std::to_string(GST_MESSAGE_TYPE(message)));
        break;
    }
    }

    /* we want to be notified again if there is a message on the bus, so
     * returning true (false means we want to stop watching for messages
     * on the bus and our callback should not be called again)
     */
    return true;
}

/*
 * Its a Basic window: copying buffer to cairo surface.
 */
void CameraImpl::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);
    auto cr = painter.context();
    if (m_camerasample)
    {
        gst_sample_ref(m_camerasample);
        GstBuffer* buffer = gst_sample_get_buffer(m_camerasample);

        GstMapInfo map;
        if (gst_buffer_map(buffer, &map, GST_MAP_READ))
        {
            Rect box = m_interface.box();
            cairo_surface_t* surface = cairo_image_surface_create_for_data(map.data,
                                       CAIRO_FORMAT_RGB16_565,
                                       box.w,
                                       box.h,
                                       cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, box.w));

            if (cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS)
            {
                cairo_set_source_surface(cr.get(), surface, box.x, box.y);
                cairo_rectangle(cr.get(), box.x, box.y, box.w, box.h);
                cairo_fill_preserve(cr.get());
                cairo_surface_destroy(surface);
            }
            gst_buffer_unmap(buffer, &map);
        }
        gst_sample_unref(m_camerasample);
    }
}

GstFlowReturn CameraImpl::on_new_buffer(GstElement* elt, gpointer data)
{
    auto cameraImpl = reinterpret_cast<CameraImpl*>(data);

    GstSample* sample;
    g_signal_emit_by_name(elt, "pull-sample", &sample);
    if (sample)
    {
#ifdef HAVE_LIBPLANES
        if (cameraImpl->m_interface.flags().is_set(Widget::flag::plane_window))
        {
            GstBuffer* buffer = gst_sample_get_buffer(sample);
            if (buffer)
            {
                GstMapInfo map;
                if (gst_buffer_map(buffer, &map, GST_MAP_READ))
                {
                    auto screen =
                        reinterpret_cast<detail::KMSOverlay*>(cameraImpl->m_interface.screen());
                    assert(screen);
                    memcpy(screen->raw(), map.data, map.size);
                    screen->schedule_flip();
                    gst_buffer_unmap(buffer, &map);
                }
            }
            gst_sample_unref(sample);
        }
        else
#endif
        {
            asio::post(Application::instance().event().io(), [cameraImpl, sample]()
            {
                cameraImpl->m_camerasample = sample;
                cameraImpl->m_interface.damage();
            });
        }
        return GST_FLOW_OK;
    }
    return GST_FLOW_ERROR;
}

#define V4L2_KMSSINK_PIPE "v4l2src device=%s ! videoconvert ! video/x-raw,width=%d,height=%d,format=%s,framerate=15/1 ! "\
						  " g1kmssink gem-name=%d "

#define V4L2_APPSINK_PIPE "v4l2src device=%s ! videoconvert ! video/x-raw,width=%d,height=%d,format=%s,framerate=15/1 ! " \
						  " appsink name=appsink async=false enable-last-sample=false sync=true"

bool CameraImpl::start()
{
    char buffer[2048];

#ifdef HAVE_LIBPLANES
    if (m_interface.flags().is_set(Widget::flag::plane_window))
    {
        auto s = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
        assert(s);

        pixel_format format = detail::egt_format(s->get_plane_format());
        SPDLOG_DEBUG("CameraWindow: egt_format = {}", format);

        if (m_usekmssink)
        {
            int gem_name = s->gem();

            if (format == pixel_format::argb8888 || format == pixel_format::xrgb8888 || format == pixel_format::rgb565)
                sprintf(buffer, V4L2_KMSSINK_PIPE, m_devnode.c_str(), m_rect.w, m_rect.h, "BGRx", gem_name);
            else if (format == pixel_format::yuv420)
                sprintf(buffer, V4L2_KMSSINK_PIPE, m_devnode.c_str(), m_rect.w, m_rect.h, "I420", gem_name);
            else if (format == pixel_format::yuyv)
                sprintf(buffer, V4L2_KMSSINK_PIPE, m_devnode.c_str(), m_rect.w, m_rect.h, "YUY2", gem_name);
        }
        else
        {
            if (format == pixel_format::argb8888 || format == pixel_format::xrgb8888 || format == pixel_format::rgb565)
                sprintf(buffer, V4L2_APPSINK_PIPE, m_devnode.c_str(), m_rect.w, m_rect.h, "BGRx");
            else if (format == pixel_format::yuv420)
                sprintf(buffer, V4L2_APPSINK_PIPE, m_devnode.c_str(), m_rect.w, m_rect.h, "I420");
            else if (format == pixel_format::yuyv)
                sprintf(buffer, V4L2_APPSINK_PIPE, m_devnode.c_str(), m_rect.w, m_rect.h, "YUY2");
        }
    }
    else
#endif
    {
        sprintf(buffer, V4L2_APPSINK_PIPE, m_devnode.c_str(), m_rect.w, m_rect.h, "RGB16");
    }

    SPDLOG_DEBUG("CameraWindow:  {}", std::string(buffer));

    /* Make sure we don't leave orphan references */
    stop();

    GError* error = nullptr;
    m_pipeline = gst_parse_launch(buffer, &error);
    if (!m_pipeline)
    {
        spdlog::error("CameraWindow: failed to create pipeline");
        m_err_message = error->message;
        return false;
    }

    if (!m_usekmssink)
    {
        m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "appsink");
        if (!m_appsink)
        {
            spdlog::error("CameraWindow: failed to get app sink element");
            m_err_message = "failed to get app sink element";
            return false;
        }

        g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, NULL);
        g_signal_connect(m_appsink, "new-sample", G_CALLBACK(on_new_buffer), this);
    }


    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(bus, &bus_callback, this);
    gst_object_unref(bus);

    int ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        SPDLOG_DEBUG("CameraWindow: set pipeline to play state failed");
        m_err_message = "failed to set pipeline to play state";
        stop();
        return false;
    }
    return true;
}

void CameraImpl::stop()
{
    if (m_pipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == ret)
        {
            spdlog::error("CameraWindow: set pipeline to NULL state failed");
        }
        g_object_unref(m_pipeline);
        m_pipeline = nullptr;
    }
}

std::string CameraImpl::get_error_message() const
{
    return m_err_message;
}

} // End of details

CameraWindow::CameraWindow(const Rect& rect, const std::string& device,
                           pixel_format format, windowhint hint)
    : Window(rect, flags_type(), format, hint)
{
    m_cameraImpl.reset(new detail::CameraImpl((*this), rect, device, detail::is_target_sama5d4()));
    detail::init_gst_thread();
}

CameraWindow::CameraWindow(const Size& size, const std::string& device,
                           pixel_format format, windowhint hint)
    : CameraWindow(Rect(size), device, format, hint)
{
}

void CameraWindow::draw(Painter& painter, const Rect& rect)
{
    m_cameraImpl->draw(painter, rect);
}

bool CameraWindow::start()
{
    return m_cameraImpl->start();
}

std::string CameraWindow::get_error_message() const
{
    return m_cameraImpl->get_error_message();
}

void CameraWindow::stop()
{
    m_cameraImpl->stop();
}

CameraWindow::~CameraWindow()
{

}

} //namespace v1

} //namespace egt
