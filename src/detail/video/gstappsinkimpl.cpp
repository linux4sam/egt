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
#include "egt/app.h"
#include "egt/detail/filesystem.h"
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
    /**
     * check for cache file by finding a playback plugin.
     * if gst_registry_find_plugin returns NULL, then no
     * cache file present and assume GSTREAMER1_PLUGIN_REGISTRY
     * is disabled in gstreamer package.
     */
    if (!gst_registry_find_plugin(gst_registry_get(), "playback"))
    {
        EGTLOG_DEBUG("manually loading gstreamer plugins");
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
            "libgstaudioconvert.so",
            "libgstalsa.so",
            "libgstlibav.so",
            "libgstvideoparsersbad.so",
        };

        std::string path;
        if (std::getenv("GST_PLUGIN_SYSTEM_PATH"))
        {
            path = std::getenv("GST_PLUGIN_SYSTEM_PATH");
            if (!path.empty() && (path.back() != '/'))
                path.back() = '/';
        }
        else if (detail::exists("/usr/lib/gstreamer-1.0/"))
        {
            path = "/usr/lib/gstreamer-1.0/";
        }
        else if (detail::exists("/usr/lib/x86_64-linux-gnu/gstreamer-1.0/"))
        {
            path = "/usr/lib/x86_64-linux-gnu/gstreamer-1.0/";
        }

        for (const auto& plugin : plugins)
        {
            GError* error = nullptr;
            gst_plugin_load_file(std::string(path + plugin).c_str(), &error);
            if (error)
            {
                if (error->message)
                    detail::error("load plugin error: {}", error->message);
                g_error_free(error);
            }
        }
    }
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

    static constexpr auto pipeline =
        "uridecodebin uri={} expose-all-streams=false name=video " \
        " {} video. ! queue ! videoscale ! video/x-raw,width={},height={} {} " \
        " ! appsink name=appsink video. {} ";

    return fmt::format(pipeline, m_uri, caps, m_size.width(), m_size.height(), vc, a_pipe);
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
