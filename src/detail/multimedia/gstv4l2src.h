/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_MULTIMEDIA_GSTV4L2SRC_H
#define EGT_SRC_DETAIL_MULTIMEDIA_GSTV4L2SRC_H

#include <egt/detail/meta.h>
#include <string>

#include "detail/multimedia/gstsrc.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

class GstDecoderImpl;

class GstV4l2src : public GstSrc
{
public:
    GstV4l2src(GstDecoderImpl& gst_decoder, const std::string& device);

    EGT_NODISCARD std::string description() const override;
    EGT_NODISCARD bool has_audio() const override
    {
        return false;
    }

private:
    std::string m_device;
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
