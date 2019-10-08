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
    if (!start_discoverer())
    {
        Event event(eventid::event2);
        m_interface.invoke_handlers(event);
        return false;
    }
#endif

    std::string buffer = create_pipeline(uri, m_audiodevice);
    SPDLOG_DEBUG("VideoWindow: {}", buffer);

    GError* error = nullptr;
    m_pipeline = gst_parse_launch(buffer.c_str(), &error);
    if (!m_pipeline)
    {
        SPDLOG_DEBUG("VideoWindow: gst_parse_launch failed ");
        if (error && error->message)
            m_err_message = error->message;
        Event event(eventid::event2);
        m_interface.invoke_handlers(event);
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

} // End of namespace detail

} // End of inline namespace v1

} //End of namespace egt
