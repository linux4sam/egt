/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_VIDEO_GSTAPPSINKIMPL_H
#define EGT_DETAIL_VIDEO_GSTAPPSINKIMPL_H

#include "detail/video/gstdecoderimpl.h"
#include <gst/app/gstappsink.h>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

class GstAppSinkImpl: public GstDecoderImpl
{
public:
    explicit GstAppSinkImpl(VideoWindow& interface, const Size& size);

    virtual bool set_media(const std::string& uri) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual std::string create_pipeline(const std::string& uri, bool m_audiodevice) override;

    virtual void scale(float scalex, float scaley) override;

    virtual ~GstAppSinkImpl() = default;

protected:
    GstElement* m_appsink;

    GstSample* m_videosample{nullptr};

    static GstFlowReturn on_new_buffer(GstElement* elt, gpointer data);

    static gboolean post_position(gpointer data);
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
