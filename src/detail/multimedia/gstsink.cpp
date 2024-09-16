/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gstsink.h"

#include "detail/egtlog.h"
#include "detail/multimedia/gstdecoderimpl.h"
#include "detail/multimedia/gstappsink.h"
#include "detail/multimedia/gstfilesink.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

GstSink::GstSink(GstDecoderImpl& gst_decoder, const Size& size, PixelFormat format)
    : m_gst_decoder(gst_decoder),
      m_size(size),
      m_format(format)
{
}

bool GstSink::post_initialize()
{
    EGTLOG_DEBUG("GstSink::post_initialize: retrieve vcaps element");
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    m_vcapsfilter = gst_bin_get_by_name(GST_BIN(m_gst_decoder.m_pipeline), "vcaps");
    if (!m_vcapsfilter)
    {
        detail::error("failed to get vcaps element");
        m_gst_decoder.on_error.invoke("failed to get vcaps element");
        return false;
    }

    return true;
}

void GstSink::size(const Size& size)
{
    std::string vs = fmt::format("video/x-raw,width={},height={},format={}",
                                 size.width(), size.height(), detail::gstreamer_format(m_format));
    GstCaps* caps = gst_caps_from_string(vs.c_str());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    g_object_set(G_OBJECT(m_vcapsfilter), "caps", caps, NULL);
    EGTLOG_DEBUG("GstSink::size: change vcaps size to {}", size);
    gst_caps_unref(caps);
}

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
