/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "detail/video/gstkmssinkimpl.h"
#include "detail/video/gstmeta.h"
#include "egt/app.h"
#include "egt/detail/filesystem.h"
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/detail/screen/kmsscreen.h"
#include "egt/types.h"
#include "egt/uri.h"
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{
GstKmsSinkImpl::GstKmsSinkImpl(VideoWindow& interface, const Size& size, bool decodertype)
    : GstDecoderImpl(interface, size),
      m_hwdecoder(decodertype)
{
    /**
     * check for cache file by finding a playback plugin.
     * if gst_registry_find_plugin returns NULL, then no
     * cache file present and assume GSTREAMER1_PLUGIN_REGISTRY
     * is disabled in gstreamer package.
     */
    if (!gst_registry_find_plugin(gst_registry_get(), "playback"))
    {
        static constexpr auto plugins =
        {
            "libgsttypefindfunctions.so",
            "libgstcoreelements.so",
            "libgstplayback.so",
            "libgstavi.so",
            "libgstisomp4.so",
            "libgstmpeg2dec.so",
            "libgstlibav.so",
            "libgstvideoparsersbad.so",
            "libgstg1.so",
            "libgstg1kmssink.so",
            "libgstaudioconvert.so",
            "libgstvolume.so",
            "libgstalsa.so",
            "libgstvideoscale.so",
            "libgstvideoconvert.so",
        };

        std::string path;
        if (std::getenv("GST_PLUGIN_SYSTEM_PATH"))
        {
            path = std::getenv("GST_PLUGIN_SYSTEM_PATH");
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

    auto s = reinterpret_cast<detail::KMSOverlay*>(interface.screen());
    assert(s);
    auto format = detail::egt_format(s->get_plane_format());
    if ((format != PixelFormat::yuyv) && (format != PixelFormat::xrgb8888))
    {
        throw std::runtime_error("Error: Invalid format: supported formats are xrgb8888 & yuyv for sama5d4");
    }
}

std::string GstKmsSinkImpl::create_pipeline()
{
    std::string format = "BGRx";
    if (m_interface.plane_window())
    {
        auto s = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
        assert(s);
        m_gem = s->gem();
        format = detail::gstreamer_format(detail::egt_format(s->get_plane_format()));
        EGTLOG_DEBUG("egt_format = {}", format);
    }

    std::string a_pipe;
    std::string caps = " caps=video/x-raw";
    if (m_audiodevice && m_audiotrack)
    {
        caps += ";audio/x-raw";
        a_pipe = " ! queue ! audioconvert ! volume name=volume ! autoaudiosink sync=false";
    }

    std::string v_pipe;
    std::array<std::string, 5> hwcodecs{"MPEG-4", "VP8", "H.264", "H.263", "H.26n"};
    auto it = std::find_if(begin(hwcodecs), end(hwcodecs), [&](const std::string & s)
    {
        return m_vcodec.find(s) != std::string::npos;
    });

    if (it != hwcodecs.end())
    {
        if (m_hwdecoder)
        {
            GstRegistry* reg = gst_registry_get();

            std::unique_ptr<GstPluginFeature, detail::GstDeleter<void, gst_object_unref>>
                    g1h264_decode{gst_registry_lookup_feature(reg, "g1h264dec")};
            if (g1h264_decode)
            {
                gst_plugin_feature_set_rank(g1h264_decode.get(), GST_RANK_PRIMARY + 1);
            }

            std::unique_ptr<GstPluginFeature, detail::GstDeleter<void, gst_object_unref>>
                    g1mp4_decode{gst_registry_lookup_feature(reg, "g1mp4dec")};
            if (g1mp4_decode)
            {
                gst_plugin_feature_set_rank(g1mp4_decode.get(), GST_RANK_PRIMARY + 1);
            }

            std::unique_ptr<GstPluginFeature, detail::GstDeleter<void, gst_object_unref>>
                    g1vp8_decode{gst_registry_lookup_feature(reg, "g1vp8dec")};
            if (g1vp8_decode)
            {
                gst_plugin_feature_set_rank(g1vp8_decode.get(), GST_RANK_PRIMARY + 1);
            }
        }

        v_pipe = fmt::format(" ! video/x-raw,width={},height={},format={}", m_size.width(), m_size.height(), format);
    }
    else
    {
        EGTLOG_DEBUG("Decoding through software decoders");
        v_pipe = fmt::format(" ! queue ! videoscale ! video/x-raw,width={},height={} " \
                             " ! videoconvert ! video/x-raw,format={}", m_size.width(), m_size.height(), format);
    }

    static constexpr auto pipeline = "uridecodebin uri={} expose-all-streams=false name=video  {} " \
                                     " video. {} ! g1kmssink gem-name={} video. {} ";

    return fmt::format(pipeline, m_uri, caps, v_pipe, m_gem, a_pipe);
}

bool GstKmsSinkImpl::media(const std::string& uri)
{
    if (detail::change_if_diff(m_uri, uri))
    {
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
                EGTLOG_DEBUG("gst_parse_launch failed : {}", error->message);
                m_interface.on_error.invoke("gst_parse_launch failed " + std::string(error->message));
            }
            return false;
        }

        EGTLOG_DEBUG("gst_parse_launch success");
        if (m_audiodevice & m_audiotrack)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
            m_volume = gst_bin_get_by_name(GST_BIN(m_pipeline), "volume");
        }

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
        gst_bus_add_watch(m_bus, &bus_callback, this);

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        m_eventsource_id = g_timeout_add(1000, (GSourceFunc) &query_position, this);

        if (!m_gmain_loop)
        {
            m_gmain_loop = g_main_loop_new(nullptr, false);
            m_gmain_thread = std::thread(g_main_loop_run, m_gmain_loop);
        }
    }
    return true;
}

gboolean GstKmsSinkImpl::query_position(gpointer data)
{
    auto impl = reinterpret_cast<GstKmsSinkImpl*>(data);

    if (!impl->m_duration)
        gst_element_query_duration(impl->m_pipeline, GST_FORMAT_TIME, &impl->m_duration);

    if (gst_element_query_position(impl->m_pipeline, GST_FORMAT_TIME, &impl->m_position))
    {
        if (Application::check_instance())
        {
            asio::post(Application::instance().event().io(), [impl]()
            {
                impl->m_interface.on_position_changed.invoke(impl->m_position);
            });
        }
    }
    return true;
}

} // End of namespace detail

} // End of inline namespace v1

} //End of namespace egt
