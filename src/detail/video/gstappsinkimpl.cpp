/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/app.h>
#include <egt/detail/video/gstappsinkimpl.h>
#include <egt/detail/screen/kmsscreen.h>
#include <egt/video.h>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

GstAppSinkImpl::GstAppSinkImpl(VideoWindow& interface, const Size& size)
    : GstDecoderImpl(size),
      m_interface(interface),
      m_appsink(NULL),
      m_videosample(NULL)
{
}

#if 0
#define APPSINKPIPE "uridecodebin uri=%s name=video caps=video/x-raw;audio/x-raw use-buffering=true buffer-size=1048576" \
					" video. ! videoscale ! video/x-raw,width=%d,height=%d " \
					" %s ! progressreport silent=true " \
					" do-query=true update-freq=1 format=time name=progress ! appsink name=appsink video." \
					" ! queue ! audioconvert ! volume name=volume ! autoaudiosink"
#else
#define APPSINKPIPE  "uridecodebin uri=%s expose-all-streams=false name=video " \
					 " caps=video/x-raw;audio/x-raw use-buffering=true buffer-size=1048576 " \
					 " video. ! queue ! videoscale  ! video/x-raw,width=%d,height=%d %s ! " \
					 " progressreport silent=true do-query=true update-freq=1 format=time name=progress ! " \
					 " appsink drop=true enable-last-sample=false caps=video/x-raw name=appsink " \
					 " video. ! queue ! audioconvert ! volume name=volume ! " \
					 " alsasink async=false enable-last-sample=false sync=true"
#endif

void GstAppSinkImpl::top_draw()
{
#ifdef HAVE_LIBPLANES
    auto screen = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
    assert(screen);
    screen->apply();
#endif
}

void GstAppSinkImpl::draw(Painter& painter, const Rect& rect)
{
    DBG("In : " << __func__);
    ignoreparam(rect);
    /*
     * its a Basic window copying buffer to Cairo surface.
     */
    auto cr = painter.context();
    if (m_videosample)
    {
        gst_sample_ref(m_videosample);
        GstBuffer* buffer = gst_sample_get_buffer(m_videosample);
        if (buffer)
        {
            GstMapInfo map;
            if (gst_buffer_map(buffer, &map, GST_MAP_READ))
            {
                Rect box = m_interface.m_box;
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
        }
        gst_sample_unref(m_videosample);
    }
}

GstFlowReturn GstAppSinkImpl::on_new_buffer(GstElement* elt, gpointer data)
{
    auto _this = reinterpret_cast<GstAppSinkImpl*>(data);
    GstSample* sample;
    g_signal_emit_by_name(elt, "pull-sample", &sample);
    if (sample)
    {
#ifdef HAVE_LIBPLANES
        if (_this->m_interface.flags().is_set(Widget::flag::plane_window))
        {
            GstBuffer* buffer = gst_sample_get_buffer(sample);
            if (buffer)
            {
                GstMapInfo map;
                if (gst_buffer_map(buffer, &map, GST_MAP_READ))
                {
                    auto screen =
                        reinterpret_cast<detail::KMSOverlay*>(_this->m_interface.screen());
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
                       std::bind(&VideoWindow::draw_frame, &_this->m_interface, sample));
        }
        return GST_FLOW_OK;
    }
    return GST_FLOW_ERROR;
}

bool GstAppSinkImpl::set_media(const std::string& uri)
{
    GError* error = NULL;

    /* Make sure we don't leave orphan references */
    destroyPipeline();

    char buffer[2048];
    std::string vc = "! videoconvert ! video/x-raw,format=";
#ifdef HAVE_LIBPLANES
    if (m_interface.flags().is_set(Widget::flag::plane_window))
    {
        auto s = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
        assert(s);
        pixel_format format = detail::egt_format(s->get_plane_format());
        DBG("VideoWindow: egt_format = " << format);
        if (format == pixel_format::yuv420)
            sprintf(buffer, APPSINKPIPE, (std::string("file://") + uri).c_str(), m_width, m_height, "");
        else if (format == pixel_format::yuyv)
            sprintf(buffer, APPSINKPIPE, (std::string("file://") + uri).c_str(), m_width, m_height, (std::string(vc + "YUY2")).c_str());
        else
            sprintf(buffer, APPSINKPIPE, (std::string("file://") + uri).c_str(), m_width, m_height, (std::string(vc + "BGRx")).c_str());
    }
    else
#endif
    {
        sprintf(buffer, APPSINKPIPE, (std::string("file://") + uri).c_str(), m_width, m_height, (std::string(vc + "RGB16")).c_str());
    }

    std::string pipe(buffer);
    DBG("VideoWindow: " << pipe);

    m_pipeline = gst_parse_launch(pipe.c_str(), &error);
    if (!m_pipeline)
    {
        m_pipeline = NULL;
        ERR("VideoWindow: failed to create video pipeline");
        return false;
    }

    m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "appsink");
    if (!m_appsink)
    {
        ERR("VideoWindow: failed to get app sink element");
        return false;
    }

    m_volume = gst_bin_get_by_name(GST_BIN(m_pipeline), "volume");
    if (!m_volume)
    {
        ERR("VideoWindow: failed to get volume element");
        return false;
    }

    g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, NULL);
    g_signal_connect(m_appsink, "new-sample", G_CALLBACK(on_new_buffer), this);

    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(bus, &bus_callback, this);
    gst_object_unref(bus);

    return true;
}

void GstAppSinkImpl::set_scale(float value)
{
#ifdef HAVE_LIBPLANES
    auto screen = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
    assert(screen);
    screen->set_scale(value);
#endif
}

float GstAppSinkImpl::scale()
{
#ifdef HAVE_LIBPLANES
    auto screen = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
    assert(screen);
    return screen->scale();
#else
    return 1.0;
#endif
}

void GstAppSinkImpl::move(const Point& p)
{
#ifdef HAVE_LIBPLANES
    KMSOverlay* screen = reinterpret_cast<KMSOverlay*>(m_interface.screen());
    assert(screen);
    screen->set_position(DisplayPoint(p.x, p.y));
#endif
}

gboolean GstAppSinkImpl::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    GError* error;
    gchar* debug;

    auto _this = reinterpret_cast<GstAppSinkImpl*>(data);

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        gst_message_parse_error(message, &error, &debug);
        _this->m_err_message = error->message;
        DBG("VideoWindow: GST_MESSAGE_ERROR from element " <<  message->src << "  " << error->message);
        DBG("VideoWindow: GST_MESSAGE_ERROR Debugging info: " << (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);
        asio::post(main_app().event().io(), std::bind(&VideoWindow::handle_gst_events,
                   &_this->m_interface, gsteventid::gst_error));
        break;
    }
    case GST_MESSAGE_WARNING:
        gst_message_parse_warning(message, &error, &debug);
        DBG("VideoWindow: GST_MESSAGE_WARNING from element " << message->src << "  " << error->message);
        DBG("VideoWindow: GST_MESSAGE_WARNING Debugging info: " << (debug ? debug : "none"));
        g_error_free(error);
        g_free(debug);
        break;
    case GST_MESSAGE_INFO:
    {
        gchar* name = gst_object_get_path_string(GST_MESSAGE_SRC(message));
        gst_message_parse_info(message, &error, &debug);
        DBG("VideoWindow: GST_MESSAGE_INFO: " << error->message);
        if (debug)
        {
            DBG("VideoWindow: GST_MESSAGE_INFO: \n" << debug << "\n");
        }
        g_clear_error(&error);
        g_free(debug);
        g_free(name);
        break;
    }
    break;
    case GST_MESSAGE_CLOCK_PROVIDE:
        DBG("VideoWindow: GST_MESSAGE_CLOCK_PROVIDE");
        break;
    case GST_MESSAGE_CLOCK_LOST:
        DBG("VideoWindow: GST_MESSAGE_CLOCK_LOST");
        break;
    case GST_MESSAGE_NEW_CLOCK:
        DBG("VideoWindow: GST_MESSAGE_NEW_CLOCK");
        break;
    case GST_MESSAGE_EOS:
    {
        DBG("VideoWindow: GST_MESSAGE_EOS: LoopMode: " << (_this->m_interface.m_loopback ? " TRUE" : "FALSE"));
        if (_this->m_interface.m_loopback)
        {
            gst_element_seek(_this->m_pipeline, 1.0, GST_FORMAT_TIME,
                             GST_SEEK_FLAG_FLUSH,
                             GST_SEEK_TYPE_SET, 0,
                             GST_SEEK_TYPE_NONE, -1);

            gst_element_set_state(_this->m_pipeline, GST_STATE_PLAYING);
        }
        else
        {
            asio::post(main_app().event().io(), std::bind(&VideoWindow::handle_gst_events,
                       &_this->m_interface, gsteventid::gst_stop));
        }
        break;
    }
    case GST_MESSAGE_PROGRESS:
        DBG("VideoWindow: GST_MESSAGE_PROGRESS");
        break;
    case GST_MESSAGE_DURATION_CHANGED:
        DBG("VideoWindow: GST_MESSAGE_DURATION_CHANGED");
        break;
    case GST_MESSAGE_ELEMENT:
    {
        const GstStructure* info = gst_message_get_structure(message);
        if (!std::string(gst_structure_get_name(info)).compare("progress"))
        {
            gint64 pos, len;
            if (gst_element_query_position(_this->m_pipeline, GST_FORMAT_TIME, &pos)
                && gst_element_query_duration(_this->m_pipeline, GST_FORMAT_TIME, &len))
            {
                _this->m_position = pos;
                _this->m_duration = len;
            }
            asio::post(main_app().event().io(), std::bind(&VideoWindow::handle_gst_events,
                       &_this->m_interface, gsteventid::gst_progress));
        }
        break;
    }
    default:
        break;
    }
    /* we want to be notified again the next time there is a message
     * on the bus, so returning TRUE (FALSE means we want to stop watching
     * for messages on the bus and our callback should not be called again)
     */
    //DBG("VideoWindow: Done " << __func__);
    return TRUE;
}

} // End of detail

} // End of inline namespace v1

} //End of namespace egt
