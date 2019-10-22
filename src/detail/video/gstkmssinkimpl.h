/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_VIDEO_GSTKMSSINKIMPL_H
#define EGT_DETAIL_VIDEO_GSTKMSSINKIMPL_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/video/gstdecoderimpl.h"

#ifdef HAVE_GSTREAMER_PBUTILS
#include <gst/pbutils/pbutils.h>
#endif

namespace egt
{
inline namespace v1
{
namespace detail
{
class GstKmsSinkImpl: public GstDecoderImpl
{
public:
    explicit GstKmsSinkImpl(VideoWindow& interface, const Size& size, bool decodertype);

    bool set_media(const std::string& uri);

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        ignoreparam(painter);
        ignoreparam(rect);
    }

    virtual void scale(float scalex, float scaley) override
    {
        ignoreparam(scalex);
        ignoreparam(scaley);
    }

    virtual std::string create_pipeline(const std::string& uri, bool m_audiodevice) override;

    virtual ~GstKmsSinkImpl() = default;

protected:
    int m_gem{-1};
    bool m_hwdecoder{false};
    std::string m_vcodec;
    std::string m_acodec;
    bool m_audiotrack{false};
    std::string m_container;
#ifdef HAVE_GSTREAMER_PBUTILS
    bool start_discoverer();

    void get_stream_info(GstDiscovererStreamInfo* info);
#endif
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
