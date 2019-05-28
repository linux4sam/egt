/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_GST_KMS_SINK_IMPL_H
#define EGT_GST_KMS_SINK_IMPL_H

#include <egt/detail/video/gstdecoderimpl.h>

namespace egt
{
inline namespace v1
{
namespace detail
{
class GstKmsSinkImpl: public GstDecoderImpl
{
public:
    GstKmsSinkImpl(VideoWindow& interface, const Size& size, bool decodertype);

    bool set_media(const std::string& uri);

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        ignoreparam(painter);
        ignoreparam(rect);
    }

protected:
    int m_gem;

    bool m_hwdecoder;
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
