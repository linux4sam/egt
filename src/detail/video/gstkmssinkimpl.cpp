/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <detail/video/gstkmssinkimpl.h>
#include <egt/app.h>
#include <egt/detail/screen/kmsoverlay.h>
#include <egt/detail/screen/kmsscreen.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

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
    if (m_hwdecoder)
    {
        GstRegistry* reg = gst_registry_get();

        std::shared_ptr<GstPluginFeature> g1h264_decode =
            std::shared_ptr<GstPluginFeature>(gst_registry_lookup_feature(reg, "g1h264dec"), gst_object_unref);
        if (g1h264_decode.get())
        {
            gst_plugin_feature_set_rank(g1h264_decode.get(), GST_RANK_PRIMARY + 1);
        }

        std::shared_ptr<GstPluginFeature> g1mp4_decode =
            std::shared_ptr<GstPluginFeature>(gst_registry_lookup_feature(reg, "g1mp4dec"), gst_object_unref);
        if (g1mp4_decode.get())
        {
            gst_plugin_feature_set_rank(g1mp4_decode.get(), GST_RANK_PRIMARY + 1);
        }

        std::shared_ptr<GstPluginFeature> g1vp8_decode =
            std::shared_ptr<GstPluginFeature>(gst_registry_lookup_feature(reg, "g1vp8dec"), gst_object_unref);
        if (g1vp8_decode.get())
        {
            gst_plugin_feature_set_rank(g1vp8_decode.get(), GST_RANK_PRIMARY + 1);
        }
    }
}

std::string GstKmsSinkImpl::create_pipeline(const std::string& uri, bool m_audiodevice)
{
    if (m_interface.flags().is_set(Widget::flag::plane_window))
    {
        auto s = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
        assert(s);
        m_gem = s->gem();
    }

    std::string a_pipe;
    if (m_audiodevice && m_audiotrack)
        a_pipe = "! queue ! audioconvert ! volume name=volume ! alsasink async=false enable-last-sample=false sync=false";
    else
        a_pipe = "";

    std::ostringstream v_pipe;
    std::array<std::string, 5> hwcodecs{"MPEG-4", "VP8", "H.264", "H.263", "H.26n"};
    auto it = std::find_if(begin(hwcodecs), end(hwcodecs), [&](const std::string & s)
    {
        return m_vcodec.find(s) != std::string::npos;
    });

    if (it != hwcodecs.end())
    {
        v_pipe << " ! video/x-raw,width=" << m_size.width() << ",height=" << m_size.height() << ",format=BGRx";
    }
    else
    {
        SPDLOG_DEBUG("VideoWindow: Decoding through software decoders");
        v_pipe << " ! queue ! videoscale ! video/x-raw,width=" << m_size.width() << ",height=" << m_size.height() <<
               " ! videoconvert ! video/x-raw,format=BGRx";
    }

    std::ostringstream pipeline;
    pipeline << "uridecodebin uri=file://" << uri << " expose-all-streams=false name=video"
             " caps=video/x-raw video." << v_pipe.str()  << " ! progressreport "
             " silent=true do-query=true update-freq=1 format=time name=progress ! g1kmssink "
             " gem-name=" << m_gem << " video. " << a_pipe ;

    return pipeline.str();
}

bool GstKmsSinkImpl::set_media(const std::string& uri)
{
    m_uri = uri;

    destroyPipeline();

#ifdef HAVE_GSTREAMER_PBUTILS
    start_discoverer();
#endif

    std::string buffer = create_pipeline(uri, m_audiodevice);
    SPDLOG_DEBUG("VideoWindow: {}", buffer);

    m_pipeline = gst_parse_launch(buffer.c_str(), nullptr);
    if (!m_pipeline)
    {
        SPDLOG_DEBUG("VideoWindow: gst_parse_launch failed ");
        return false;
    }

    SPDLOG_DEBUG("VideoWindow: gst_parse_launch success");
    if (m_audiodevice)
    {
        m_volume = gst_bin_get_by_name(GST_BIN(m_pipeline), "volume");
    }

    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(m_bus, &bus_callback, this);

    return play();
}

#ifdef HAVE_GSTREAMER_PBUTILS
bool GstKmsSinkImpl::start_discoverer()
{
    GError* err1 = nullptr;
    std::shared_ptr<GstDiscoverer> discoverer =
        std::shared_ptr<GstDiscoverer>(gst_discoverer_new(5 * GST_SECOND, &err1), g_object_unref);
    if (!discoverer.get())
    {
        SPDLOG_DEBUG("VideoWindow: Error creating discoverer instance: {}", err1->message);
        g_clear_error(&err1);
        return false;
    }

    GError* err2 = nullptr;
    std::shared_ptr<GstDiscovererInfo> info = std::shared_ptr<GstDiscovererInfo>(
                gst_discoverer_discover_uri(discoverer.get(), ("file://" + m_uri).c_str(), &err2),
                g_object_unref);
    if (!info.get())
    {
        GstDiscovererResult result = gst_discoverer_info_get_result(info.get());
        SPDLOG_DEBUG("VideoWindow: Discoverer result: {} ", result);
        switch (result)
        {
        case GST_DISCOVERER_URI_INVALID:
        {
            SPDLOG_DEBUG("VideoWindow: Discoverer Invalid URI {} ", m_uri);
            return false;
        }
        case GST_DISCOVERER_ERROR:
        {
            SPDLOG_DEBUG("VideoWindow: Discoverer error: {} ", err2->message);
            return false;
        }
        case GST_DISCOVERER_TIMEOUT:
        {
            SPDLOG_DEBUG("VideoWindow: Discoverer Timeout");
            return false;
        }
        case GST_DISCOVERER_BUSY:
        {
            SPDLOG_DEBUG("VideoWindow: Discoverer Busy");
            return false;
        }
        case GST_DISCOVERER_MISSING_PLUGINS:
        {
            const GstStructure* s = gst_discoverer_info_get_misc(info.get());
            std::shared_ptr<gchar> str = std::shared_ptr<gchar>(gst_structure_to_string(s), g_free);
            SPDLOG_DEBUG("VideoWindow: Discoverer Missing plugins: {}", str.get());
            return false;
        }
        case GST_DISCOVERER_OK:
            SPDLOG_DEBUG("VideoWindow: should not be here");
            return false;
        }
    }

    std::shared_ptr<GstDiscovererStreamInfo> sinfo = std::shared_ptr<GstDiscovererStreamInfo>(
                gst_discoverer_info_get_stream_info(info.get()), g_object_unref);
    if (!sinfo.get())
    {
        SPDLOG_DEBUG("VideoWindow: Discoverer error: get_stream_info failed");
        return false;
    }

    std::shared_ptr<GList> streams = std::shared_ptr<GList>(
                                         gst_discoverer_container_info_get_streams(GST_DISCOVERER_CONTAINER_INFO(sinfo.get())),
                                         gst_discoverer_stream_info_list_free);
    if (!streams.get())
    {
        SPDLOG_DEBUG("VideoWindow: Discoverer error: failed to container_info");
        return false;
    }

    GList* cinfo = streams.get();
    while (cinfo)
    {
        GstDiscovererStreamInfo* tmpinf = (GstDiscovererStreamInfo*)cinfo->data;
        get_stream_info(tmpinf);
        cinfo = cinfo->next;
    }
    return true;
}

/* Print information regarding a stream */
void GstKmsSinkImpl::get_stream_info(GstDiscovererStreamInfo* info)
{
    std::shared_ptr<GstCaps> caps = std::shared_ptr<GstCaps>(
                                        gst_discoverer_stream_info_get_caps(info),
                                        gst_caps_unref);
    if (caps.get())
    {
        std::shared_ptr<gchar> desc;
        if (gst_caps_is_fixed(caps.get()))
            desc.reset(gst_pb_utils_get_codec_description(caps.get()), g_free);
        else
            desc.reset(gst_caps_to_string(caps.get()), g_free);

        if (desc.get())
        {
            std::string type = std::string(gst_discoverer_stream_info_get_stream_type_nick(info));
            if (!type.compare("video"))
            {
                m_vcodec = desc.get();
            }
            else if (!type.compare("audio"))
            {
                m_acodec = desc.get();
                m_audiotrack = true;
            }
            else if (!type.compare("container"))
            {
                m_container = desc.get();
            }
            SPDLOG_DEBUG("VideoWindow: {} : {}", type, std::string(desc.get()));
        }
    }
}
#endif

} // End of namespace detail

} // End of inline namespace v1

} //End of namespace egt
