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
    if (m_audiodevice)
        a_pipe = "! queue ! audioconvert ! volume name=volume ! alsasink async=false enable-last-sample=false sync=false";
    else
        a_pipe = "";

    std::ostringstream pipeline;
    pipeline << "uridecodebin uri=file://" << uri << " expose-all-streams=false name=video"
             " caps=video/x-raw video. ! video/x-raw,width=" <<  std::to_string(m_size.w) <<
             ",height=" << std::to_string(m_size.h) << ",format=BGRx " << " ! progressreport "
             " silent=true do-query=true update-freq=1 format=time name=progress ! g1kmssink "
             " gem-name=" << std::to_string(m_gem) << " video. " << a_pipe ;

    return pipeline.str();
}

bool GstKmsSinkImpl::set_media(const std::string& uri)
{
    m_uri = uri;

    destroyPipeline();

    if (m_hwdecoder)
    {
        GstRegistry* reg = gst_registry_get();

        GstPluginFeature* g1h264_decode = gst_registry_lookup_feature(reg, "g1h264dec");
        if (g1h264_decode)
        {
            gst_plugin_feature_set_rank(g1h264_decode, GST_RANK_PRIMARY + 1);
            gst_object_unref(g1h264_decode);
        }

        GstPluginFeature* g1mp4_decode = gst_registry_lookup_feature(reg, "g1mp4dec");
        if (g1mp4_decode)
        {
            gst_plugin_feature_set_rank(g1mp4_decode, GST_RANK_PRIMARY + 1);
            gst_object_unref(g1mp4_decode);
        }

        GstPluginFeature* g1vp8_decode = gst_registry_lookup_feature(reg, "g1vp8dec");
        if (g1vp8_decode)
        {
            gst_plugin_feature_set_rank(g1vp8_decode, GST_RANK_PRIMARY + 1);
            gst_object_unref(g1vp8_decode);
        }
    }

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

} // End of namespace detail

} // End of inline namespace v1

} //End of namespace egt
