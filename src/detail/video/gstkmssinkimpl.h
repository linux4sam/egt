/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_VIDEO_GSTKMSSINKIMPL_H
#define EGT_SRC_DETAIL_VIDEO_GSTKMSSINKIMPL_H

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

    bool media(const std::string& uri) override;

    void draw(Painter& painter, const Rect& rect) override
    {
        ignoreparam(painter);
        ignoreparam(rect);
    }

    void scale(float scalex, float scaley) override
    {
        ignoreparam(scalex);
        ignoreparam(scaley);
    }

    std::string create_pipeline() override;

    virtual ~GstKmsSinkImpl() = default;

protected:
    int m_gem{-1};
    bool m_hwdecoder{false};

    static gboolean query_position(gpointer data);
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
