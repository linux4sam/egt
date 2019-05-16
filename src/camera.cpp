/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/app.h>
#include <egt/camera.h>
#include "egt/detail/screen/kmsscreen.h"
#include <egt/video.h>

#include <string>
#include <thread>
#include <fstream>

using namespace std;

namespace egt
{
inline namespace v1
{

CameraWindow::CameraWindow(const Size& size, const std::string& device, pixel_format format, windowhint hint)
    : Window(size, flags_type(), format, hint),
      m_devnode(device),
      m_pipeline(NULL),
      m_appsink(NULL),
      m_camerasample(NULL)
{
    gst_init(NULL, NULL);
    detail::init_gst_thread();
}

CameraWindow::CameraWindow(const Rect& rect, const std::string& device, pixel_format format, windowhint hint)
    : Window(rect, flags_type(), format, hint),
      m_devnode(device),
      m_pipeline(NULL),
      m_appsink(NULL),
      m_camerasample(NULL)
{
    gst_init(NULL, NULL);
    detail::init_gst_thread();
}

void CameraWindow::top_draw()
{
#ifdef HAVE_LIBPLANES
    auto screen = reinterpret_cast<detail::KMSOverlay*>(this->screen());
    assert(screen);
    screen->apply();
#endif
}

void CameraWindow::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);
    /*
     * its a Basic window copying buffer to Cairo surface.
     */
    auto cr = painter.context();
    if (m_camerasample)
    {
        gst_sample_ref(m_camerasample);
        GstBuffer* buffer = gst_sample_get_buffer(m_camerasample);

        GstMapInfo map;
        if (gst_buffer_map(buffer, &map, GST_MAP_READ))
        {
            cairo_surface_t* surface = cairo_image_surface_create_for_data(map.data,
                                       CAIRO_FORMAT_RGB16_565,
                                       m_box.w,
                                       m_box.h,
                                       cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, m_box.w));

            if (cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS)
            {
                cairo_set_source_surface(cr.get(), surface, m_box.x, m_box.y);
                cairo_rectangle(cr.get(), m_box.x, m_box.y, m_box.w, m_box.h);
                cairo_fill_preserve(cr.get());
                cairo_surface_destroy(surface);
            }
            gst_buffer_unmap(buffer, &map);
        }
        gst_sample_unref(m_camerasample);
    }
}

void CameraWindow::copy_frame(GstSample* sample)
{
    m_camerasample = sample;
    Window::damage();
}

GstFlowReturn CameraWindow::on_new_buffer(GstElement* elt, gpointer data)
{
    GstSample* sample;

    auto _this = reinterpret_cast<CameraWindow*>(data);

    g_signal_emit_by_name(elt, "pull-sample", &sample);
    if (sample)
    {
#ifdef HAVE_LIBPLANES
        if (_this->flags().is_set(Widget::flag::plane_window))
        {
            GstBuffer* buffer = gst_sample_get_buffer(sample);
            if (buffer)
            {
                GstMapInfo map;
                if (gst_buffer_map(buffer, &map, GST_MAP_READ))
                {
                    auto screen =
                        reinterpret_cast<detail::KMSOverlay*>(_this->screen());
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
            asio::post(main_app().event().io(),
                       std::bind(&CameraWindow::copy_frame, _this, sample));
        }
        return GST_FLOW_OK;
    }
    return GST_FLOW_ERROR;
}

#define V4L2_KMSSINK_PIPE "v4l2src device=%s ! videoconvert ! video/x-raw,width=%d,height=%d,format=%s,framerate=15/1 ! g1kmssink gem-name=%d"

#define V4L2_APPSINK_PIPE "v4l2src device=%s ! videoconvert ! video/x-raw,width=%d,height=%d,format=%s,framerate=15/1 ! " \
						  " appsink name=appsink async=false enable-last-sample=false sync=true"

bool CameraWindow::start()
{
    GError* error = NULL;
    GstBus* bus;

    char buffer[2048];
    if (flags().is_set(Widget::flag::plane_window))
    {
#ifdef HAVE_LIBPLANES
        auto s = reinterpret_cast<detail::KMSOverlay*>(this->screen());
        assert(s);

        pixel_format format = detail::egt_format(s->get_plane_format());
        DBG("CameraWindow: egt_format = " << format);

        if (detail::is_target_sama5d4())
        {
            int gem_name = s->gem();

            if (format == pixel_format::argb8888 || format == pixel_format::xrgb8888 || format == pixel_format::rgb565)
                sprintf(buffer, V4L2_KMSSINK_PIPE, m_devnode.c_str(), w(), h(), "BGRx", gem_name);
            else if (format == pixel_format::yuv420)
                sprintf(buffer, V4L2_KMSSINK_PIPE, m_devnode.c_str(), w(), h(), "I420", gem_name);
            else if (format == pixel_format::yuyv)
                sprintf(buffer, V4L2_KMSSINK_PIPE, m_devnode.c_str(), w(), h(), "YUY2", gem_name);
        }
        else
        {
            if (format == pixel_format::argb8888 || format == pixel_format::xrgb8888 || format == pixel_format::rgb565)
                sprintf(buffer, V4L2_APPSINK_PIPE, m_devnode.c_str(), w(), h(), "BGRx");
            else if (format == pixel_format::yuv420)
                sprintf(buffer, V4L2_APPSINK_PIPE, m_devnode.c_str(), w(), h(), "I420");
            else if (format == pixel_format::yuyv)
                sprintf(buffer, V4L2_APPSINK_PIPE, m_devnode.c_str(), w(), h(), "YUY2");
        }
#endif
    }
    else
    {
        sprintf(buffer, V4L2_APPSINK_PIPE, m_devnode.c_str(), w(), h(), "RGB16");
    }

    string pipe(buffer);
    DBG("CameraWindow:  " << pipe);

    /* Make sure we don't leave orphan references */
    stop();

    m_pipeline = gst_parse_launch(pipe.c_str(), &error);
    if (!m_pipeline)
    {
        m_pipeline = NULL;
        ERR("CameraWindow: failed to create pipeline");
        return false;
    }

    if (!detail::is_target_sama5d4())
    {
        m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "appsink");
        if (!m_appsink)
        {
            ERR("CameraWindow: failed to get app sink element");
            return false;
        }

        g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, NULL);
        g_signal_connect(m_appsink, "new-sample", G_CALLBACK(on_new_buffer), this);
    }

    bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(bus, &bus_callback, this);
    gst_object_unref(bus);

    int ret;
    ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        DBG("CameraWindow: set pipeline to play state failed");
        stop();
        return false;
    }
    return true;
}

void CameraWindow::move(const Point& p)
{
    Window::move(p);

#ifdef HAVE_LIBPLANES
    auto screen = reinterpret_cast<detail::KMSOverlay*>(this->screen());
    assert(screen);
    screen->set_position(DisplayPoint(p.x, p.y));
#endif
}

void CameraWindow::set_scale(float value)
{
#ifdef HAVE_LIBPLANES
    auto screen = reinterpret_cast<detail::KMSOverlay*>(this->screen());
    assert(screen);
    screen->set_scale(value);
#endif
}

float CameraWindow::scale()
{
#ifdef HAVE_LIBPLANES
    auto screen = reinterpret_cast<detail::KMSOverlay*>(this->screen());
    assert(screen);
    return screen->scale();
#else
    return 1.0;
#endif
}

std::string CameraWindow::get_error_message()
{
    return m_err_message;
}

void CameraWindow::stop()
{
    if (m_pipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == ret)
        {
            ERR("CameraWindow: set pipeline to NULL state failed");
        }
        g_object_unref(m_pipeline);
        m_pipeline = NULL;
    }
}

gboolean CameraWindow::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    GError* error;
    gchar* debug;

    auto _this = reinterpret_cast<CameraWindow*>(data);

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        gst_message_parse_error(message, &error, &debug);
        _this->m_err_message = error->message;
        DBG("CameraWindow: GST_MESSAGE_ERROR from element " <<  message->src << "  " << error->message);
        DBG("CameraWindow: GST_MESSAGE_ERROR Debugging info: " << (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);

        main_app().event().io().post([_this]()
        {
            Event event(eventid::event2);
            _this->invoke_handlers(event);
        });

        break;
    }
    case GST_MESSAGE_WARNING:
        gst_message_parse_warning(message, &error, &debug);
        DBG("CameraWindow: GST_MESSAGE_WARNING from element " << message->src << "  " << error->message);
        DBG("CameraWindow: GST_MESSAGE_WARNING Debugging info: " << (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);
        break;
    case GST_MESSAGE_INFO:
    {
        gchar* name = gst_object_get_path_string(GST_MESSAGE_SRC(message));
        gst_message_parse_info(message, &error, &debug);
        DBG("CameraWindow: GST_MESSAGE_INFO: " << error->message);
        if (debug)
        {
            DBG("CameraWindow: GST_MESSAGE_INFO: \n" << debug << "\n");
        }
        g_clear_error(&error);
        g_free(debug);
        g_free(name);
        break;
    }
    break;
    case GST_MESSAGE_CLOCK_PROVIDE:
        DBG("CameraWindow: GST_MESSAGE_CLOCK_PROVIDE");
        break;
    case GST_MESSAGE_CLOCK_LOST:
        DBG("CameraWindow: GST_MESSAGE_CLOCK_LOST");
        break;
    case GST_MESSAGE_NEW_CLOCK:
        DBG("CameraWindow: GST_MESSAGE_NEW_CLOCK");
        break;
    case GST_MESSAGE_PROGRESS:
        DBG("CameraWindow: GST_MESSAGE_PROGRESS");
        break;
    case GST_MESSAGE_DURATION_CHANGED:
        DBG("CameraWindow: GST_MESSAGE_DURATION_CHANGED");
        break;
    case GST_MESSAGE_ELEMENT:
    {
        DBG("CameraWindow: GST_MESSAGE_ELEMENT");
        break;
    }
    case GST_MESSAGE_TAG:
    {
        DBG("CameraWindow: GST_MESSAGE_DURATION_CHANGED");
        break;
    }
    default:
        DBG("CameraWindow: default Message " << std::to_string(GST_MESSAGE_TYPE(message)));
        break;
    }
    /* we want to be notified again the next time there is a message
     * on the bus, so returning TRUE (FALSE means we want to stop watching
     * for messages on the bus and our callback should not be called again)
     */
    return TRUE;
}


CameraWindow::~CameraWindow()
{

}

} //namespace v1

} //namespace egt
