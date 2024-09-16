/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_MULTIMEDIA_GSTSRC_H
#define EGT_SRC_DETAIL_MULTIMEDIA_GSTSRC_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <memory>
#include <string>

#include "egt/detail/meta.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

class GstDecoderImpl;
class GstUridecodebin;
class GstV4l2src;

class GstSrc
{
public:

    static std::unique_ptr<GstSrc> create(GstDecoderImpl& gst_decoder,
                                          const std::string& input, bool enable_video, bool enable_audio);

    EGT_NODISCARD virtual std::string description() const = 0;
    EGT_NODISCARD virtual bool has_audio() const = 0;

protected:

    GstSrc(GstDecoderImpl& gst_decoder);

#ifdef HAVE_GSTREAMER_PBUTILS
    static bool start_discoverer(GstDecoderImpl& gst_decoder,
                                 const std::string& input, bool& has_audio_track);
#endif

    GstDecoderImpl& m_gst_decoder;
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
