/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gstv4l2src.h"

#include "detail/egtlog.h"
#include "detail/fmt.h"
#include "detail/multimedia/gstdecoderimpl.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

GstV4l2src::GstV4l2src(GstDecoderImpl& gst_decoder, const std::string& device)
    : GstSrc(gst_decoder),
      m_device(device)
{
    EGTLOG_DEBUG("GstV4l2src::GstV4l2src: device={}", device);
    /*
     * This method is call to display device resolutions in the logs as it can
     * be useful to set the size of the video sink.
     */
    gst_decoder.get_device_caps(device);
}

std::string GstV4l2src::description() const
{
    return fmt::format("v4l2src device={} ! tee name=source", m_device);
}

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
