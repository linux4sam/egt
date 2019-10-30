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

void GstAppSinkImpl::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);
    /*
     * its a Basic window copying buffer to Cairo surface.
     */
    if (m_videosample)
    {
        GstCaps* caps = gst_sample_get_caps(m_videosample);
        GstStructure* capsStruct = gst_caps_get_structure(caps, 0);
        int width, height;
        gst_structure_get_int(capsStruct, "width", &width);
        gst_structure_get_int(capsStruct, "height", &height);

        gst_sample_ref(m_videosample);
        GstBuffer* buffer = gst_sample_get_buffer(m_videosample);
        if (buffer)
        {
            GstMapInfo map;
            if (gst_buffer_map(buffer, &map, GST_MAP_READ))
            {
                auto box = m_interface.box();
                auto surface = unique_cairo_surface_t(
                                   cairo_image_surface_create_for_data(map.data,
                                           CAIRO_FORMAT_RGB16_565,
                                           width,
                                           height,
                                           cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, width)));

                if (cairo_surface_status(surface.get()) == CAIRO_STATUS_SUCCESS)
                {
                    auto cr = painter.context().get();
                    if (width != box.width() || height != box.height())
                    {
                        double scalex = static_cast<double>(box.width()) / width;
                        double scaley = static_cast<double>(box.height()) / height;
                        cairo_scale(cr, scalex, scaley);
                    }
                    cairo_set_source_surface(cr, surface.get(), box.x(), box.y());
                    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
                    cairo_paint(cr);
                }

                m_position = GST_BUFFER_TIMESTAMP(buffer);
                gst_buffer_unmap(buffer, &map);
            }
        }
        gst_sample_unref(m_videosample);

    }
}

GstFlowReturn GstAppSinkImpl::on_new_buffer(GstElement* elt, gpointer data)
{
    auto impl = reinterpret_cast<GstAppSinkImpl*>(data);
    GstSample* sample;
    g_signal_emit_by_name(elt, "pull-sample", &sample);
    if (sample)
    {
#ifdef HAVE_LIBPLANES
        if (impl->m_interface.flags().is_set(Widget::flag::plane_window))
        {
            GstBuffer* buffer = gst_sample_get_buffer(sample);
            if (buffer)
            {
                GstMapInfo map;
                if (gst_buffer_map(buffer, &map, GST_MAP_READ))
                {
                    auto screen =
                        reinterpret_cast<detail::KMSOverlay*>(impl->m_interface.screen());
                    assert(screen);
                    memcpy(screen->raw(), map.data, map.size);
                    screen->schedule_flip();
                    impl->m_position = GST_BUFFER_TIMESTAMP(buffer);
                    gst_buffer_unmap(buffer, &map);
                }
            }
            gst_sample_unref(sample);
        }
        else
#endif
        {
            asio::post(Application::instance().event().io(), [impl, sample]()
            {
                if (impl->m_videosample)
                    gst_sample_unref(impl->m_videosample);

                impl->m_videosample = sample;
                impl->m_interface.damage();
            });
        }
        return GST_FLOW_OK;
    }
    return GST_FLOW_ERROR;
}

std::string GstAppSinkImpl::create_pipeline(const std::string& uri, bool m_audiodevice)
{
    std::string vc = " ! videoconvert ! video/x-raw,format=";
    if (m_interface.flags().is_set(Widget::flag::plane_window))
    {
        auto s = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
        assert(s);
        pixel_format format = detail::egt_format(s->get_plane_format());
        SPDLOG_DEBUG("VideoWindow: egt_format = {}", format);

        if (format == pixel_format::yuyv)
            vc = vc + "YUY2";
        else if (format == pixel_format::rgb565)
            vc = vc + "RGB16";
        else if ((format == pixel_format::argb8888) ||
                 (format == pixel_format::xrgb8888))
            vc = vc + "BGRx";
        else if (format == pixel_format::nv21)
            vc = vc + "NV21";
        else
            vc = "";
    }
    else
    {
        vc = vc + "RGB16";
    }

    std::string a_pipe;
    std::string caps = " caps=video/x-raw";
    if (m_audiodevice && m_audiotrack)
    {
        caps += ";audio/x-raw";
        a_pipe = "! queue ! audioconvert ! volume name=volume ! alsasink async=false enable-last-sample=false sync=false";
    }

    std::ostringstream pipeline;
    pipeline << "uridecodebin uri=file://" << uri << " expose-all-streams=false name=video"
             << caps  << " video. ! queue ! videoscale ! video/x-raw,width=" <<
             std::to_string(m_size.width()) << ",height=" << std::to_string(m_size.height()) << vc <<
             " ! appsink name=appsink video. " << a_pipe ;

    return pipeline.str();
}

/* This function takes a textual representation of a pipeline
 * and create an actual pipeline
 */
bool GstAppSinkImpl::set_media(const std::string& uri)
{
    m_uri = uri;
    /* Make sure we don't leave orphan references */
    destroyPipeline();

#ifdef HAVE_GSTREAMER_PBUTILS
    if (!start_discoverer())
    {
        m_interface.invoke_handlers(eventid::error);
        return false;
    }
#endif

    std::string buffer = create_pipeline(uri, m_audiodevice);
    SPDLOG_DEBUG("VideoWindow: {}", buffer);

    GError* error = nullptr;
    m_pipeline = gst_parse_launch(buffer.c_str(), &error);
    if (!m_pipeline)
    {
        SPDLOG_ERROR("VideoWindow: failed to create video pipeline");
        if (error && error->message)
            m_err_message = error->message;
        m_interface.invoke_handlers(eventid::error);
        return false;
    }

    m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "appsink");
    if (!m_appsink)
    {
        SPDLOG_ERROR("VideoWindow: failed to get app sink element");
        m_err_message = "failed to get app sink element";
        m_interface.invoke_handlers(eventid::error);
        return false;
    }

    if (m_audiodevice && m_audiotrack)
    {
        m_volume = gst_bin_get_by_name(GST_BIN(m_pipeline), "volume");
        if (!m_volume)
        {
            SPDLOG_ERROR("VideoWindow: failed to get volume element");
            m_err_message = "failed to get volume element";
            m_interface.invoke_handlers(eventid::error);
            return false;
        }
    }

    g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, nullptr);
    g_signal_connect(m_appsink, "new-sample", G_CALLBACK(on_new_buffer), this);

    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    m_bus_watchid = gst_bus_add_watch(m_bus, &bus_callback, this);

    g_timeout_add(500, (GSourceFunc) &post_position, this);

    return true;
}

void GstAppSinkImpl::scale(float scalex, float scaley)
{
    m_interface.resize(Size(m_size.width() * scalex, m_size.height() * scaley));
}

gboolean GstAppSinkImpl::post_position(gpointer data)
{
    auto impl = reinterpret_cast<GstAppSinkImpl*>(data);

    asio::post(Application::instance().event().io(), [impl]()
    {
        impl->m_interface.invoke_handlers(eventid::property_changed);
    });

    return true;
}

} // End of detail

} // End of inline namespace v1

} //End of namespace egt
