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

#include "detail/multimedia/gstsink.h"
#include "detail/multimedia/gstsrc.h"
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
    /**
     * Event signal.
     * @{
     */
    /// Invoked when the position of the player changes.
    Signal<int64_t> on_position_changed;

    /// Invoked when an error occurs.
    Signal<const std::string&> on_error;

    /// Invoked on end of stream.
    Signal<> on_eos;

    /// Invoked when the state of the player changes.
    Signal<> on_state_changed;

    /// Invoked when a video device is connected.
    Signal<const std::string&> on_connect;

    /// Invoked when a video device is disconnected.
    Signal<const std::string&> on_disconnect;
    /** @} */

    // special functions deleted because they are never used
    GstDecoderImpl() = delete;
    GstDecoderImpl(const GstDecoderImpl&) = delete;
    GstDecoderImpl& operator=(const GstDecoderImpl&) = delete;
    GstDecoderImpl(GstDecoderImpl&&) = delete;
    GstDecoderImpl& operator=(GstDecoderImpl&&) = delete;

    explicit GstDecoderImpl(Window* window, const Size& size);

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

    virtual bool mute(bool mute);

    int64_t duration() const;

    int64_t position() const;

    virtual bool seek(int64_t time);

    virtual bool has_audio() const;

    virtual void destroyPipeline();

    bool start();

    void stop();

    void device(const std::string& device);

    std::string device() const;

    std::vector<std::string> list_devices();

    std::tuple<std::string, std::string, std::string, std::vector<std::tuple<int, int>>>
    get_device_caps(const std::string& dev_name);

    void loopback(bool enable);

    EGT_NODISCARD bool loopback() const;

    void enable_video(bool enable);

    void enable_audio(bool enable);

    void output(const std::string& file, const Size& size, PixelFormat format);

    virtual ~GstDecoderImpl();

protected:
    Window* m_window;
    GstElement* m_pipeline{nullptr};
    std::string m_custom_pipeline_desc{};
    Size m_size;
    gboolean m_audiodevice;
    int64_t m_start{0};
    int64_t m_duration{0};
    int64_t m_position{0};
    GstElement* m_volume{nullptr};
    gboolean m_seek_enabled{false};
    std::string m_uri;
    std::string m_devnode{};
    GstBus* m_bus{nullptr};
    guint m_bus_watchid{0};
    GMainLoop* m_gmain_loop{nullptr};
    std::thread m_gmain_thread;
    guint m_eventsource_id{0};
    GstElement* m_vcapsfilter{nullptr};

    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);

    static gboolean post_position(gpointer data);

    std::string m_caps_name;
    std::string m_caps_format;
    std::vector<std::tuple<int, int>> m_resolutions;
    std::vector<std::string> m_devices;
    GstDeviceMonitor* m_device_monitor{nullptr};
    std::vector<std::string> get_video_device_list();
    std::tuple<std::string, std::string, std::string, std::vector<std::tuple<int, int>>>
    get_video_device_caps(const std::string& dev_name);

    static gboolean device_monitor_bus_callback(GstBus* bus, GstMessage* message, gpointer data);

    bool m_loopback{false};

    std::unique_ptr<GstSrc> m_src;
    bool m_video_enabled{true};
    bool m_audio_enabled{true};

    std::unique_ptr<GstSink> m_sink;

    std::string m_output{};
    PixelFormat m_output_format{};

    friend class GstSink;
    friend class GstAppSink;

};

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt

#endif
