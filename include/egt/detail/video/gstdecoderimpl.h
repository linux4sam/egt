/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_VIDEO_GSTDECODERIMPL_H
#define EGT_DETAIL_VIDEO_GSTDECODERIMPL_H

#include <egt/geometry.h>
#include <egt/painter.h>
#include <egt/video.h>
#include <gst/gst.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

class GstDecoderImpl
{
public:

    GstDecoderImpl() = delete;

    GstDecoderImpl(VideoWindow& interface, const Size& size);

    virtual bool set_media(const std::string& uri) = 0;

    virtual void draw(Painter& painter, const Rect& rect) = 0;

    virtual bool play();

    virtual bool pause();

    virtual bool playing() const;

    virtual bool set_volume(double volume);

    virtual double get_volume() const;

    std::string get_error_message() const;

    int64_t get_duration() const;

    int64_t get_position() const;

    virtual bool seek(const int64_t time);

    virtual void destroyPipeline();

protected:
    VideoWindow& m_interface;
    GstElement* m_pipeline{nullptr};
    Size m_size;
    int64_t m_start{0};
    int64_t m_duration{0};
    int64_t m_position{0};
    GstElement* m_volume{nullptr};
    std::string m_err_message;
    gboolean m_seek_enabled{false};
    gboolean m_seekdone{false};

    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
