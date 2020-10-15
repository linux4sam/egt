/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/egtlog.h"
#include "detail/video/gstappsinkimpl.h"
#include "detail/video/gstmeta.h"
#include "egt/app.h"
#include "egt/types.h"
#include "egt/uri.h"
#include <string>

#ifdef HAVE_LIBPLANES
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/detail/screen/kmsscreen.h"
#endif

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
    static constexpr auto plugins =
    {
        "libgstcoreelements.so",
        "libgsttypefindfunctions.so",
        "libgstplayback.so",
        "libgstavi.so",
        "libgstisomp4.so",
        "libgstapp.so",
        "libgstmpeg2dec.so",
        "libgstvideoscale.so",
        "libgstvideoconvert.so",
        "libgstvolume.so",
        "libgstaudioparsers.so",
        "libgstaudiorate.so",
        "libgstaudioconvert.so",
        "libgstaudioresample.so",
        "libgstautodetect.so",
        "libgstalsa.so",
        "libgstlibav.so",
        "libgstvideoparsersbad.so",
    };
    detail::gst_init_plugins(plugins);
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
        int width = 0;
        int height = 0;
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
    auto impl = static_cast<GstAppSinkImpl*>(data);
    GstSample* sample;
    g_signal_emit_by_name(elt, "pull-sample", &sample);
    if (sample)
    {
#ifdef HAVE_LIBPLANES
        if (impl->m_interface.plane_window())
        {
            GstCaps* caps = gst_sample_get_caps(sample);
            GstStructure* capsStruct = gst_caps_get_structure(caps, 0);
            int width = 0;
            int height = 0;
            gst_structure_get_int(capsStruct, "width", &width);
            gst_structure_get_int(capsStruct, "height", &height);

            auto b = impl->m_interface.content_area();
            if (b.size() != egt::Size(width, height))
            {
                gst_sample_unref(sample);
                EGTLOG_DEBUG("frame dropped b.size() {} egt::Size(width, height) {}", b.size(), egt::Size(width, height));
                // drop this frame and continue;
                return GST_FLOW_OK;
            }

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
            if (Application::check_instance())
            {
                asio::post(Application::instance().event().io(), [impl, sample]()
                {
                    if (impl->m_videosample)
                        gst_sample_unref(impl->m_videosample);

                    impl->m_videosample = sample;
                    impl->m_interface.damage();
                });
            }
        }
        return GST_FLOW_OK;
    }
    return GST_FLOW_ERROR;
}

std::string GstAppSinkImpl::create_pipeline()
{
    std::string vc = " ! videoconvert ! video/x-raw,format=";
#ifdef HAVE_LIBPLANES
    if (m_interface.plane_window())
    {
        auto s = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
        assert(s);
        PixelFormat fmt = detail::egt_format(s->get_plane_format());
        EGTLOG_DEBUG("egt_format = {}", fmt);

        vc += detail::gstreamer_format(fmt);
    }
    else
#endif
    {
        vc += "RGB16";
    }

    std::string a_pipe;
    std::string caps = " caps=video/x-raw";
    if (m_audiodevice && m_audiotrack)
    {
        caps += ";audio/x-raw";
        a_pipe = "! queue ! audioconvert ! volume name=volume ! autoaudiosink sync=false";
    }

    std::string vscapf = " ! capsfilter name=vcaps";
    if ((m_size.width() > 32) && (m_size.height() > 32))
    {
        vscapf = fmt::format(vscapf + " caps=video/x-raw,width={},height={} ",
                             m_size.width(), m_size.height());
    }

    static constexpr auto pipeline =
        "uridecodebin uri={} expose-all-streams=false name=video " \
        " {} video. {} ! videoscale {} ! appsink name=appsink video. {} ";

    return fmt::format(pipeline, m_uri, caps, vc, vscapf, a_pipe);
}

/* This function takes a textual representation of a pipeline
 * and create an actual pipeline
 */
bool GstAppSinkImpl::media(const std::string& uri)
{
    if (detail::change_if_diff(m_uri, uri))
    {
        /* Make sure we don't leave orphan references */
        destroyPipeline();

#ifdef HAVE_GSTREAMER_PBUTILS
        Uri u(m_uri);
        if (u.scheme() != "rtsp")
        {
            if (!start_discoverer())
            {
                detail::error("media file discoverer failed");
                return false;
            }
        }
#endif

        const auto buffer = create_pipeline();
        EGTLOG_DEBUG("{}", buffer);

        GError* error = nullptr;
        m_pipeline = gst_parse_launch(buffer.c_str(), &error);
        if (!m_pipeline)
        {
            if (error && error->message)
            {
                detail::error("{}", error->message);
                m_interface.on_error.invoke(error->message);
            }
            return false;
        }

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        m_vcapsfilter = gst_bin_get_by_name(GST_BIN(m_pipeline), "vcaps");
        if (!m_vcapsfilter)
        {
            detail::error("failed to get vcaps element");
            m_interface.on_error.invoke("failed to get vcaps element");
            return false;
        }

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "appsink");
        if (!m_appsink)
        {
            detail::error("failed to get app sink element");
            m_interface.on_error.invoke("failed to get app sink element");
            return false;
        }

        if (m_audiodevice && m_audiotrack)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
            m_volume = gst_bin_get_by_name(GST_BIN(m_pipeline), "volume");
            if (!m_volume)
            {
                detail::error("failed to get volume element");
                m_interface.on_error.invoke("failed to get volume element");
                return false;
            }
        }

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, nullptr);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        g_signal_connect(m_appsink, "new-sample", G_CALLBACK(on_new_buffer), this);

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
        m_bus_watchid = gst_bus_add_watch(m_bus, &bus_callback, this);

        g_timeout_add(5000, static_cast<GSourceFunc>(&post_position), this);

        if (!m_gmain_loop)
        {
            m_gmain_loop = g_main_loop_new(nullptr, false);
            m_gmain_thread = std::thread(g_main_loop_run, m_gmain_loop);
        }
    }
    return true;
}

void GstAppSinkImpl::scale(float scalex, float scaley)
{
    m_interface.resize(Size(m_size.width() * scalex, m_size.height() * scaley));
}

void GstAppSinkImpl::resize(const Size& size)
{
    if (size != m_size)
    {
        std::string vs = fmt::format("video/x-raw,width={},height={}", size.width(), size.height());
        GstCaps* caps = gst_caps_from_string(vs.c_str());
        if (m_pipeline && m_vcapsfilter)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
            g_object_set(G_OBJECT(m_vcapsfilter), "caps", caps, NULL);
            EGTLOG_DEBUG("change gst videoscale element to {}", size);
        }
        gst_caps_unref(caps);

        if (m_size.empty())
        {
            m_size = size;
            EGTLOG_DEBUG("setting m_size to {} from {}", size, m_size);
        }
    }
}

gboolean GstAppSinkImpl::post_position(gpointer data)
{
    auto impl = static_cast<GstAppSinkImpl*>(data);

    if (Application::check_instance())
    {
        asio::post(Application::instance().event().io(), [impl]()
        {
            impl->m_interface.on_position_changed.invoke(impl->m_position);
        });
    }

    return true;
}

} // End of detail

} // End of inline namespace v1

} //End of namespace egt
