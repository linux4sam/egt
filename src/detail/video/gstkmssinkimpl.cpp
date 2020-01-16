/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/video/gstkmssinkimpl.h"
#include "detail/video/gstmeta.h"
#include "egt/app.h"
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/detail/screen/kmsscreen.h"
#include "egt/types.h"
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
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
        auto plugins =
        {
            "/usr/lib/gstreamer-1.0/libgstcoreelements.so",
            "/usr/lib/gstreamer-1.0/libgsttypefindfunctions.so",
            "/usr/lib/gstreamer-1.0/libgstplayback.so",
            "/usr/lib/gstreamer-1.0/libgstavi.so",
            "/usr/lib/gstreamer-1.0/libgstisomp4.so",
            "/usr/lib/gstreamer-1.0/libgstmpeg2dec.so",
            "/usr/lib/gstreamer-1.0/libgstlibav.so",
            "/usr/lib/gstreamer-1.0/libgstvideoparsersbad.so",
            "/usr/lib/gstreamer-1.0/libgstg1.so",
            "/usr/lib/gstreamer-1.0/libgstg1kmssink.so",
            "/usr/lib/gstreamer-1.0/libgstaudioconvert.so",
            "/usr/lib/gstreamer-1.0/libgstvolume.so",
            "/usr/lib/gstreamer-1.0/libgstalsa.so",
            "/usr/lib/gstreamer-1.0/libgstvideoscale.so",
            "/usr/lib/gstreamer-1.0/libgstvideoconvert.so",
        };

        for (auto& plugin : plugins)
        {
            GError* error = nullptr;
            gst_plugin_load_file(plugin, &error);
            if (error)
            {
                if (error->message)
                    spdlog::error("load plugin error: {}", error->message);
                g_error_free(error);
            }
        }
    }

    auto s = reinterpret_cast<detail::KMSOverlay*>(interface.screen());
    assert(s);
    auto format = detail::egt_format(s->get_plane_format());
    if ((format != PixelFormat::yuyv) && (format != PixelFormat::xrgb8888))
    {
        std::ostringstream ss;
        ss << "Error: Invalid format: supported formats are xrgb8888 & yuyv for sama5d4";
        throw std::runtime_error(ss.str());
    }
}

std::string GstKmsSinkImpl::create_pipeline(const std::string& uri, bool m_audiodevice)
{
    std::string vc = "BGRx";
    if (m_interface.flags().is_set(Widget::Flag::plane_window))
    {
        auto s = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
        assert(s);
        m_gem = s->gem();
        auto format = detail::egt_format(s->get_plane_format());
        SPDLOG_DEBUG("egt_format = {}", format);

        vc = detail::gstreamer_format(format);
    }

    std::string a_pipe;
    std::string caps = " caps=video/x-raw";
    if (m_audiodevice && m_audiotrack)
    {
        caps += ";audio/x-raw";
        a_pipe = " ! queue ! audioconvert ! volume name=volume ! alsasink async=false enable-last-sample=false sync=false";
    }

    std::ostringstream v_pipe;
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

        v_pipe << " ! video/x-raw,width=" << m_size.width() << ",height=" << m_size.height() << ",format=" << vc;
    }
    else
    {
        SPDLOG_DEBUG("Decoding through software decoders");
        v_pipe << " ! queue ! videoscale ! video/x-raw,width=" << m_size.width() << ",height=" << m_size.height() <<
               " ! videoconvert ! video/x-raw,format=" << vc;
    }

    std::ostringstream pipeline;
    pipeline << "uridecodebin uri=file://" << uri << " expose-all-streams=false name=video"
             << caps << " video." << v_pipe.str()  << " ! g1kmssink gem-name=" << m_gem
             << " video. " << a_pipe ;

    return pipeline.str();
}

bool GstKmsSinkImpl::media(const std::string& uri)
{
    m_uri = uri;

    destroyPipeline();

#ifdef HAVE_GSTREAMER_PBUTILS
    if (!start_discoverer())
    {
        spdlog::error("{}", m_err_message);
        m_interface.on_error.invoke();
        return false;
    }
#endif

    std::string buffer = create_pipeline(uri, m_audiodevice);
    SPDLOG_DEBUG("{}", buffer);

    GError* error = nullptr;
    m_pipeline = gst_parse_launch(buffer.c_str(), &error);
    if (!m_pipeline)
    {
        SPDLOG_DEBUG("gst_parse_launch failed ");
        if (error && error->message)
            m_err_message = error->message;
        m_interface.on_error.invoke();
        return false;
    }

    SPDLOG_DEBUG("gst_parse_launch success");
    if (m_audiodevice & m_audiotrack)
    {
        m_volume = gst_bin_get_by_name(GST_BIN(m_pipeline), "volume");
    }

    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(m_bus, &bus_callback, this);

    g_timeout_add(1000, (GSourceFunc) &query_position, this);

    return true;
}

gboolean GstKmsSinkImpl::query_position(gpointer data)
{
    auto impl = reinterpret_cast<GstKmsSinkImpl*>(data);

    if (!impl->m_duration)
        gst_element_query_duration(impl->m_pipeline, GST_FORMAT_TIME, &impl->m_duration);

    if (gst_element_query_position(impl->m_pipeline, GST_FORMAT_TIME, &impl->m_position))
    {
        asio::post(Application::instance().event().io(), [impl]()
        {
            impl->m_interface.on_position_changed.invoke();
        });
    }
    return true;
}

} // End of namespace detail

} // End of inline namespace v1

} //End of namespace egt
