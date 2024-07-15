/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_MULTIMEDIA_GSTDECODERIMPL_H
#define EGT_SRC_DETAIL_MULTIMEDIA_GSTDECODERIMPL_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/geometry.h"
#include "egt/painter.h"
#include "egt/video.h"
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

    // special functions deleted because they are never used
    GstDecoderImpl() = delete;
    GstDecoderImpl(const GstDecoderImpl&) = delete;
    GstDecoderImpl& operator=(const GstDecoderImpl&) = delete;
    GstDecoderImpl(GstDecoderImpl&&) = delete;
    GstDecoderImpl& operator=(GstDecoderImpl&&) = delete;

    explicit GstDecoderImpl(VideoWindow& iface, const Size& size);

    bool media(const std::string& uri);

    void draw(Painter& painter, const Rect& rect);

    void custom_pipeline(const std::string& pipeline_desc);

    std::string create_pipeline_desc();

    bool create_pipeline(const std::string& pipeline_desc);

    void scale(float scalex, float scaley);

    virtual void resize(const Size& size);

    virtual bool play();

    virtual bool pause();

    virtual bool playing() const;

    virtual bool volume(int volume);

    virtual int volume() const;

    int64_t duration() const;

    int64_t position() const;

    virtual bool seek(int64_t time);

    virtual bool has_audio() const;

    virtual void destroyPipeline();

    virtual ~GstDecoderImpl();

protected:
    VideoWindow& m_interface;
    GstElement* m_pipeline{nullptr};
    std::string m_custom_pipeline_desc{};
    Size m_size;
    gboolean m_audiodevice;
    int64_t m_start{0};
    int64_t m_duration{0};
    int64_t m_position{0};
    GstElement* m_volume{nullptr};
    gboolean m_seek_enabled{false};
    gboolean m_seekdone{false};
    std::string m_uri;
    GstBus* m_bus{nullptr};
    guint m_bus_watchid{0};
    GMainLoop* m_gmain_loop{nullptr};
    std::thread m_gmain_thread;
    guint m_eventsource_id{0};
    bool m_audiotrack{false};
    GstElement* m_vcapsfilter{nullptr};

    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);

#ifdef HAVE_GSTREAMER_PBUTILS
    bool start_discoverer();
#endif

    GstElement* m_appsink{nullptr};
    GstSample* m_videosample{nullptr};
    static GstFlowReturn on_new_buffer(GstElement* elt, gpointer data);
    static gboolean post_position(gpointer data);
};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
