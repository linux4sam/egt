/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_VIDEO_GSTKMSSINKIMPL_H
#define EGT_DETAIL_VIDEO_GSTKMSSINKIMPL_H

#include "detail/video/gstdecoderimpl.h"

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

    bool set_media(const std::string& uri) override;

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
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
