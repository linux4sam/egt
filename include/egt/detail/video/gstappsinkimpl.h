/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_GST_APP_SINK_IMPL_H
#define EGT_GST_APP_SINK_IMPL_H

#include <egt/detail/video/gstdecoderimpl.h>

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
    GstAppSinkImpl(VideoWindow& interface, const Size& size);

    virtual bool set_media(const std::string& uri) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

protected:
    GstElement* m_appsink;

    GstSample* m_videosample{nullptr};

    static GstFlowReturn on_new_buffer(GstElement* elt, gpointer data);
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
