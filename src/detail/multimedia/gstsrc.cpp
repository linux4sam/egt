/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gstsrc.h"

#include "detail/egtlog.h"
#include "detail/fmt.h"
#include "detail/multimedia/gstmeta.h"
#include "detail/multimedia/gstdecoderimpl.h"
#include "detail/multimedia/gsturidecodebin.h"
#include "detail/multimedia/gstv4l2src.h"
#include "egt/uri.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

GstSrc::GstSrc(GstDecoderImpl& gst_decoder)
    : m_gst_decoder(gst_decoder)
{
}

std::unique_ptr<GstSrc> GstSrc::create(GstDecoderImpl& gst_decoder,
    const std::string& input, bool enable_video, bool enable_audio)
{
    EGTLOG_DEBUG("GstSrc::create: input={}, enable_video={}, enable_audio={}",
                 input, enable_video, enable_audio);

    const auto pos = input.find("/dev/video");
    if (pos == std::string::npos)
    {
#ifdef HAVE_GSTREAMER_PBUTILS
        Uri u(input);
        if (u.scheme() != "rtsp")
        {
            auto has_audio_track = bool{};
            if (!start_discoverer(gst_decoder, input, has_audio_track))
            {
                detail::error("media file discoverer failed");
                return nullptr;
            }
            enable_audio &= has_audio_track;
        }
#endif
        EGTLOG_DEBUG("GstSrc::create: GstUridecodebin");
        return std::make_unique<GstUridecodebin>(gst_decoder, input, enable_video, enable_audio);
    }
    else
    {
        /*
         * Extract the device path. In the case of a video device, for
         * the pipeline description, we just want the device path and not
         * an uri.
         */
        const auto device = std::string{input.begin() + pos, input.end()};
        EGTLOG_DEBUG("GstSrc::create: GstV4l2src");
        return std::make_unique<GstV4l2src>(gst_decoder, device);
    }
}

#ifdef HAVE_GSTREAMER_PBUTILS
bool GstSrc::start_discoverer(GstDecoderImpl& gst_decoder, const std::string& input, bool& has_audio_track)
{
    has_audio_track = false;

    GError* err1 = nullptr;
    std::unique_ptr<GstDiscoverer, GstDeleter<void, g_object_unref>>
            discoverer{gst_discoverer_new(5 * GST_SECOND, &err1)};
    if (!discoverer)
    {
        detail::error("error creating discoverer instance: {} ", std::string(err1->message));
        gst_decoder.on_error.invoke("error creating discoverer instance: " + std::string(err1->message));
        g_clear_error(&err1);
        return false;
    }

    GError* err2 = nullptr;
    std::unique_ptr<GstDiscovererInfo, GstDeleter<void, g_object_unref>>
            info{gst_discoverer_discover_uri(discoverer.get(), input.c_str(), &err2)};

    GstDiscovererResult result = gst_discoverer_info_get_result(info.get());
    EGTLOG_DEBUG("GstSrc::start_discoverer: result: {}", result);
    switch (result)
    {
    case GST_DISCOVERER_URI_INVALID:
    {
        detail::error("invalid URI: {}", input);
        gst_decoder.on_error.invoke("invalid URI: " + input);
        return false;
    }
    case GST_DISCOVERER_ERROR:
    {
        detail::error("error: {} ", std::string(err2->message));
        gst_decoder.on_error.invoke("error: " + std::string(err2->message));
        break;
    }
    case GST_DISCOVERER_TIMEOUT:
    {

        detail::error("gst discoverer timeout");
        gst_decoder.on_error.invoke("gst discoverer timeout");
        return false;
    }
    case GST_DISCOVERER_BUSY:
    {
        detail::error("gst discoverer busy");
        gst_decoder.on_error.invoke("gst discoverer busy");
        return false;
    }
    case GST_DISCOVERER_MISSING_PLUGINS:
    {
        const GstStructure* s = gst_discoverer_info_get_misc(info.get());
        GstStringHandle str{gst_structure_to_string(s)};
        gst_decoder.on_error.invoke(str.get());
        return false;
    }
    case GST_DISCOVERER_OK:
        EGTLOG_DEBUG("GstSrc::start_discoverer: discovery successful");
        break;
    }

    GList* audio_streams = gst_discoverer_info_get_audio_streams(info.get());
    if (audio_streams)
    {
        has_audio_track = true;
        gst_discoverer_stream_info_list_free(audio_streams);
    }

    return true;
}
#endif

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
