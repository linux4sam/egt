/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_VIDEO_GSTDECODERIMPL_H
#define EGT_DETAIL_VIDEO_GSTDECODERIMPL_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/geometry.h>
#include <egt/painter.h>
#include <egt/video.h>
#include <gst/gst.h>

#ifdef HAVE_GSTREAMER_PBUTILS
#include <gst/pbutils/pbutils.h>
#endif

#include <thread>

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

    explicit GstDecoderImpl(VideoWindow& interface, const Size& size);

    virtual bool set_media(const std::string& uri) = 0;

    virtual void draw(Painter& painter, const Rect& rect) = 0;

    virtual std::string create_pipeline(const std::string& uri, bool m_audiodevice) = 0;

    virtual void scale(float scalex, float scaley) = 0;

    virtual bool play();

    virtual bool pause();

    virtual bool playing() const;

    virtual bool set_volume(double volume);

    virtual double volume() const;

    std::string error_message() const;

    int64_t duration() const;

    int64_t position() const;

    virtual bool seek(int64_t time);

    virtual void destroyPipeline();

    virtual ~GstDecoderImpl();

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
    gboolean m_audiodevice{true};
    std::string m_uri;
    GstBus* m_bus{nullptr};
    guint m_bus_watchid{0};
    GMainLoop* m_gmainLoop{nullptr};
    std::thread m_gmainThread;
    std::string m_vcodec;
    std::string m_acodec;
    bool m_audiotrack{false};
    std::string m_container;

    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);

#ifdef HAVE_GSTREAMER_PBUTILS
    bool start_discoverer();
    void get_stream_info(GstDiscovererStreamInfo* info);
#endif
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
