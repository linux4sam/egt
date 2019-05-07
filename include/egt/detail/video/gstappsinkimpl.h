/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_GST_APP_SINK_IMPL_H
#define EGT_GST_APP_SINK_IMPL_H

#include <egt/detail/video/gstdecoderimpl.h>
#include <egt/video.h>

#include <gst/app/gstappsink.h>

#include <string>
#include <vector>

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

    bool set_media(const std::string& filename);

    void draw(Painter& painter, const Rect& rect);

    void top_draw();

    float scale() override;

    void set_scale(float value) override;

    void move(const Point& point);

    void push_buffer(GstSample* sample) override
    {
        m_videosample = sample;
    }

protected:
    VideoWindow& m_interface;

    GstElement* m_appsink;

    GstSample* m_videosample;

    static GstFlowReturn on_new_buffer(GstElement* elt, gpointer data);

    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
