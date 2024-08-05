/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gsturidecodebin.h"

#include "detail/fmt.h"
#include "detail/multimedia/gstdecoderimpl.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

GstUridecodebin::GstUridecodebin(GstDecoderImpl& gst_decoder, const std::string& uri,
                                 bool video, bool audio)
    : GstSrc(gst_decoder),
      m_uri(uri),
      m_video(video),
      m_audio(audio)
{
}

std::string GstUridecodebin::description() const
{
    /*
     * GstURIDecodeBin caps are propagated to GstDecodeBin. Its caps must a be
     * a superset of the inner decoder element. So make this superset large
     * enough to contain the inner decoder's caps that have features.
     */
    auto caps = std::string{};
    if (m_video && m_audio)
        caps = "video/x-raw(ANY);audio/x-raw(ANY)";
    else if (m_video)
        caps = "video/x-raw(ANY)";
    else if (m_audio)
        caps = "audio/x-raw(ANY)";

    return fmt::format("uridecodebin uri={} caps={} name=source", m_uri, caps);
}

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
