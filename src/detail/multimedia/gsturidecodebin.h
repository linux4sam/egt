/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_MULTIMEDIA_GSTURIDECODEBIN_H
#define EGT_SRC_DETAIL_MULTIMEDIA_GSTURIDECODEBIN_H

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

class GstUridecodebin : public GstSrc
{
public:

    GstUridecodebin(GstDecoderImpl& gst_decoder, const std::string& uri,
                    bool video, bool audio);

    EGT_NODISCARD std::string description() const override;
    EGT_NODISCARD bool has_audio() const override
    {
        return m_audio;
    }

private:

    std::string m_uri;
    bool m_video;
    bool m_audio;
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
