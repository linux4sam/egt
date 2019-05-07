/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_GST_KMS_SINK_IMPL_H
#define EGT_GST_KMS_SINK_IMPL_H

#include <egt/detail/video/gstdecoderimpl.h>
#include <egt/video.h>

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

    bool set_media(const std::string& filename);

    void top_draw();

    void move(const Point& p);

    float scale() override;

    void set_scale(float value) override;

    void draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(painter);
        ignoreparam(rect);
    }

protected:
    int m_gem;

    VideoWindow& m_interface;

    bool m_hwdecoder;

    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
