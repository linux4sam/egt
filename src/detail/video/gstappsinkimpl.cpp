/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/video/gstappsinkimpl.h"
#include <egt/app.h>
#include <egt/detail/screen/kmsoverlay.h>
#include <egt/detail/screen/kmsscreen.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

GstAppSinkImpl::GstAppSinkImpl(VideoWindow& interface, const Size& size)
    : GstDecoderImpl(interface, size),
      m_appsink(nullptr)
{
}

#define APPSINKPIPE  "uridecodebin uri=%s expose-all-streams=false name=video " \
					 " caps=video/x-raw;audio/x-raw use-buffering=true buffer-size=1048576 " \
					 " video. ! queue ! videoscale  ! video/x-raw,width=%d,height=%d %s ! " \
					 " progressreport silent=true do-query=true update-freq=1 format=time name=progress ! " \
					 " appsink drop=true enable-last-sample=false caps=video/x-raw name=appsink " \
					 " video. ! queue ! audioconvert ! volume name=volume ! " \
					 " alsasink async=false enable-last-sample=false sync=true"

void GstAppSinkImpl::draw(Painter& painter, const Rect& rect)
{
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
        }
        gst_sample_unref(m_videosample);
    }
}

GstFlowReturn GstAppSinkImpl::on_new_buffer(GstElement* elt, gpointer data)
{
    auto Impl = reinterpret_cast<GstAppSinkImpl*>(data);
    GstSample* sample;
    g_signal_emit_by_name(elt, "pull-sample", &sample);
    if (sample)
    {
#ifdef HAVE_LIBPLANES
        if (Impl->m_interface.flags().is_set(Widget::flag::plane_window))
        {
            GstBuffer* buffer = gst_sample_get_buffer(sample);
            if (buffer)
            {
                GstMapInfo map;
                if (gst_buffer_map(buffer, &map, GST_MAP_READ))
                {
                    auto screen =
                        reinterpret_cast<detail::KMSOverlay*>(Impl->m_interface.screen());
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
            asio::post(Application::instance().event().io(), [Impl, sample]()
            {
                Impl->m_videosample = sample;
                Impl->m_interface.damage();
            });
        }
        return GST_FLOW_OK;
    }
    return GST_FLOW_ERROR;
}

/* This function takes a textual representation of a pipeline
 * and create an actual pipeline
 */

bool GstAppSinkImpl::set_media(const std::string& uri)
{
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
        SPDLOG_DEBUG("VideoWindow: egt_format = {}", format);
        if (format == pixel_format::yuv420)
            sprintf(buffer, APPSINKPIPE, (std::string("file://") + uri).c_str(), m_size.w, m_size.h, "");
        else if (format == pixel_format::yuyv)
            sprintf(buffer, APPSINKPIPE, (std::string("file://") + uri).c_str(), m_size.w, m_size.h, (std::string(vc + "YUY2")).c_str());
        else
            sprintf(buffer, APPSINKPIPE, (std::string("file://") + uri).c_str(), m_size.w, m_size.h, (std::string(vc + "BGRx")).c_str());
    }
    else
#endif
    {
        sprintf(buffer, APPSINKPIPE, (std::string("file://") + uri).c_str(), m_size.w, m_size.h, (std::string(vc + "RGB16")).c_str());
    }

    SPDLOG_DEBUG("VideoWindow: {}", std::string(buffer));

    GError* error = nullptr;
    m_pipeline = gst_parse_launch(buffer, &error);
    if (!m_pipeline)
    {
        spdlog::error("VideoWindow: failed to create video pipeline");
        m_err_message = error->message;
        return false;
    }

    m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "appsink");
    if (!m_appsink)
    {
        spdlog::error("VideoWindow: failed to get app sink element");
        m_err_message = "failed to get app sink element";
        return false;
    }

    m_volume = gst_bin_get_by_name(GST_BIN(m_pipeline), "volume");
    if (!m_volume)
    {
        spdlog::error("VideoWindow: failed to get volume element");
        return false;
    }

    g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, NULL);
    g_signal_connect(m_appsink, "new-sample", G_CALLBACK(on_new_buffer), this);

    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    m_bus_watchid = gst_bus_add_watch(m_bus, &bus_callback, this);

    return true;
}

} // End of detail

} // End of inline namespace v1

} //End of namespace egt
