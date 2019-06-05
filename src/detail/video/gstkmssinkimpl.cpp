/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <detail/video/gstkmssinkimpl.h>
#include <egt/app.h>
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

#define HARDWARE_PIPE "uridecodebin uri=%s expose-all-streams=false name=srcVideo " \
					 " caps=video/x-raw srcVideo. ! video/x-raw,width=%d,height=%d,format=BGRx ! "\
					 " progressreport silent=true do-query=true update-freq=1 format=time " \
					 " name=progress ! g1kmssink gem-name=%d srcVideo. ! queue ! audioconvert ! " \
					 " volume name=volume ! alsasink async=false enable-last-sample=false sync=true "

#define SOFTWARE_PIPE  "uridecodebin uri=%s expose-all-streams=false name=video " \
					 " caps=video/x-raw video. ! queue ! videoscale  ! video/x-raw,width=%d,height=%d %s ! " \
					 " progressreport silent=true do-query=true update-freq=1 format=time name=progress ! " \
					 " g1kmssink gem-name=%d video. ! queue ! audioconvert ! volume name=volume ! " \
					 " alsasink async=false enable-last-sample=false sync=true"

bool GstKmsSinkImpl::set_media(const std::string& uri)
{
    GError* error = NULL;

    destroyPipeline();

    KMSOverlay* s = reinterpret_cast<KMSOverlay*>(m_interface.screen());
    assert(s);
    m_gem = s->gem();
    pixel_format format = detail::egt_format(s->get_plane_format());
    SPDLOG_DEBUG("VideoWindow: egt_format = {}", format);

    char buffer[2048];
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

        if (uri.find("https://") != std::string::npos)
        {
            sprintf(buffer, HARDWARE_PIPE, uri.c_str(), m_size.w, m_size.h, m_gem);
        }
        else
        {
            sprintf(buffer, HARDWARE_PIPE, (std::string("file://") + uri).c_str(), m_size.w, m_size.h, m_gem);
        }
    }
    else
    {
        std::string vc = "! videoconvert ! video/x-raw,format=";
        if (format == pixel_format::yuv420)
            sprintf(buffer, SOFTWARE_PIPE, (std::string("file://") + uri).c_str(), m_size.w, m_size.h, "", m_gem);
        else if (format == pixel_format::yuyv)
            sprintf(buffer, SOFTWARE_PIPE, (std::string("file://") + uri).c_str(), m_size.w, m_size.h, (std::string(vc + "YUY2")).c_str(), m_gem);
        else
            sprintf(buffer, SOFTWARE_PIPE, (std::string("file://") + uri).c_str(), m_size.w, m_size.h, (std::string(vc + "BGRx")).c_str(), m_gem);
    }

    SPDLOG_DEBUG("VideoWindow: {}", std::string(buffer));

    m_pipeline = gst_parse_launch(buffer, &error);
    if (m_pipeline)
    {
        SPDLOG_DEBUG("VideoWindow: Create pipeline success");

        m_volume = gst_bin_get_by_name(GST_BIN(m_pipeline), "volume");

        GstBus* bus;
        bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
        gst_bus_add_watch(bus, &bus_callback, this);
        gst_object_unref(bus);
        return true;
    }
    return false;
}

} // End of namespace detail

} // End of inline namespace v1

} //End of namespace egt
