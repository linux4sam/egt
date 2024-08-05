/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gstv4l2src.h"

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
}

std::string GstV4l2src::description() const
{
    return fmt::format("v4l2src device={} ! tee name=source", m_device);
}

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
