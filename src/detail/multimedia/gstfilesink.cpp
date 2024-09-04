/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gstfilesink.h"

#include "detail/egtlog.h"
#include "detail/multimedia/gstdecoderimpl.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

GstFileSink::GstFileSink(GstDecoderImpl& gst_decoder, const Size& size,
                         PixelFormat format, const std::string& file)
    : GstSink(gst_decoder, size, format),
      m_output(file)
{
}

std::string GstFileSink::description()
{
    const auto video_caps_filter =
        fmt::format("caps=video/x-raw,width={},height={},format={}",
                    m_size.width(), m_size.height(), detail::gstreamer_format(m_format));
    return fmt::format("videoconvert ! videoscale ! capsfilter name=vcaps {} ! avimux ! filesink location={}",
                       video_caps_filter, m_output);
}

void GstFileSink::draw(Painter& painter, const Rect& rect)
{
}

bool GstFileSink::post_initialize()
{
    return true;
}

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

