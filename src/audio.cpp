/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/video/gstmeta.h"
#include "egt/app.h"
#include "egt/audio.h"
#include "egt/detail/filesystem.h"
#include "egt/detail/meta.h"
#include "egt/respath.h"
#include "egt/video.h"
#include <exception>
#include <gst/gst.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <thread>

namespace egt
{
inline namespace v1
{
namespace detail
{

struct AudioPlayerImpl
{
    explicit AudioPlayerImpl(AudioPlayer& player)
        : player(player)
    {}

    /**
     * Set the current state of the stream.
     */
    bool change_state(AudioPlayer* player, GstState state)
    {
        if (m_pipeline)
        {
            GstStateChangeReturn ret = gst_element_set_state(m_pipeline, state);
            if (GST_STATE_CHANGE_FAILURE == ret)
            {
                spdlog::error("unable to set audio pipeline to {}", state);
                player->on_error.invoke("audio pipeline set state failed");
                return false;
            }
        }
        else
        {
            player->on_error.invoke("audio pipeline not yet initialized");
            return false;
        }

        return true;
    }


    AudioPlayer& player;
    GstElement* m_pipeline {nullptr};
    GstElement* m_volume {nullptr};
    gint64 m_position {0};
    gint64 m_duration {0};
    std::string m_uri;
    int m_volume_value {100};
    GMainLoop* m_gmain_loop {nullptr};
    std::thread m_gmain_thread;
    guint m_eventsource_id {0};
};

}

inline uint64_t nsec_to_sec(uint64_t s)
{
    return (s / 1000000000ULL);
}

static gboolean query_position(gpointer data)
{
    auto impl = static_cast<detail::AudioPlayerImpl*>(data);

    /* don't query position if not playing */
    if (!impl->player.playing())
        return true;

    if (gst_element_query_position(impl->m_pipeline, GST_FORMAT_TIME, &impl->m_position) &&
        gst_element_query_duration(impl->m_pipeline, GST_FORMAT_TIME, &impl->m_duration))
    {
        SPDLOG_TRACE("position: {}  && duration: {}", impl->m_position, impl->m_duration);

        if (Application::check_instance())
        {
            asio::post(Application::instance().event().io(), [impl]()
            {
                impl->player.on_position_changed.invoke(nsec_to_sec(impl->m_position));
            });
        }
    }
    return true;
}

static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    detail::ignoreparam(bus);

    auto impl = static_cast<detail::AudioPlayerImpl*>(data);

    SPDLOG_TRACE("gst message: {}", GST_MESSAGE_TYPE_NAME(message));

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        detail::GstErrorHandle error;
        detail::GstStringHandle debug;
        detail::gst_message_parse(gst_message_parse_error, message, error, debug);
        if (error)
        {
            SPDLOG_DEBUG("gst error: {} {}",
                         error.get()->message,
                         debug ? debug.get() : "");

            if (Application::check_instance())
            {
                asio::post(Application::instance().event().io(), [impl, error = std::move(error)]()
                {
                    impl->player.on_error.invoke(error->message);
                });
            }
        }
        break;
    }
    case GST_MESSAGE_WARNING:
    {
        detail::GstErrorHandle error;
        detail::GstStringHandle debug;
        detail::gst_message_parse(gst_message_parse_warning, message, error, debug);
        if (error)
        {
            SPDLOG_DEBUG("gst warning: {} {}",
                         error.get()->message,
                         debug ? debug.get() : "");
        }
        break;
    }
    case GST_MESSAGE_INFO:
    {
        detail::GstErrorHandle error;
        detail::GstStringHandle debug;
        detail::gst_message_parse(gst_message_parse_info, message, error, debug);
        if (error)
        {
            SPDLOG_DEBUG("gst info: {} {}",
                         error.get()->message,
                         debug ? debug.get() : "");
        }
        break;
    }
    case GST_MESSAGE_EOS:
    {
        gst_element_seek(impl->m_pipeline, 1.0, GST_FORMAT_TIME,
                         GST_SEEK_FLAG_FLUSH,
                         GST_SEEK_TYPE_SET, 0,
                         GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);

        if (Application::check_instance())
        {
            asio::post(Application::instance().event().io(), [impl]()
            {
                impl->player.pause();
                impl->player.on_eos.invoke();
            });
        }

        break;
    }
    case GST_MESSAGE_STATE_CHANGED:
    {
        GstState old_state;
        GstState new_state;
        GstState pending_state;
        gst_message_parse_state_changed(message, &old_state, &new_state, &pending_state);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        if (GST_MESSAGE_SRC(message) == GST_OBJECT(impl->m_pipeline))
        {
            SPDLOG_DEBUG("state changed from {} to {}",
                         gst_element_state_get_name(old_state),
                         gst_element_state_get_name(new_state));

            if (Application::check_instance())
            {
                asio::post(Application::instance().event().io(), [impl]()
                {
                    impl->player.on_state_changed.invoke();
                });
            }
        }
        break;
    }
    default:
        break;
    }

    /* we want to be notified again the next time there is a message
     * on the bus, so returning TRUE (FALSE means we want to stop watching
     * for messages on the bus and our callback should not be called again)
     */
    return true;
}

AudioPlayer::AudioPlayer()
    : m_impl(std::make_unique<detail::AudioPlayerImpl>(*this))
{
    if (!detail::audio_device())
        throw std::runtime_error("no soundcards");

    GError* err = nullptr;
    if (!gst_init_check(nullptr, nullptr, &err))
    {
        std::ostringstream ss;
        ss << "failed to initialize gstreamer: ";
        if (err && err->message)
        {
            ss << err->message;
            g_error_free(err);
        }
        else
        {
            ss << "unknown error";
        }

        throw std::runtime_error(ss.str());
    }
}

AudioPlayer::AudioPlayer(const std::string& uri)
    : AudioPlayer()
{
    if (!media(uri))
    {
        throw std::runtime_error("failed to initalize gstreamer pipeline");
    }
}

void AudioPlayer::destroyPipeline()
{
    if (m_impl->m_pipeline)
        (void)gst_element_set_state(m_impl->m_pipeline, GST_STATE_NULL);

    if (m_impl->m_volume)
    {
        g_object_unref(m_impl->m_volume);
        m_impl->m_volume = nullptr;
    }

    if (m_impl->m_pipeline)
    {
        g_object_unref(m_impl->m_pipeline);
        m_impl->m_pipeline = nullptr;
    }

    if (m_impl->m_eventsource_id > 0)
    {
        g_source_remove(m_impl->m_eventsource_id);
    }

    if (m_impl->m_gmain_loop)
    {
        // check loop is running to avoid race condition when stop is called too early
        if (g_main_loop_is_running(m_impl->m_gmain_loop))
        {
            //stop loop and wait
            g_main_loop_quit(m_impl->m_gmain_loop);
        }
        m_impl->m_gmain_thread.join();
        g_main_loop_unref(m_impl->m_gmain_loop);
        m_impl->m_gmain_loop = nullptr;
    }
}

AudioPlayer::~AudioPlayer() noexcept
{
    destroyPipeline();
}

bool AudioPlayer::play()
{
    return m_impl->change_state(this, GST_STATE_PLAYING);
}

bool AudioPlayer::pause()
{
    return m_impl->change_state(this, GST_STATE_PAUSED);
}

bool AudioPlayer::null()
{
    return m_impl->change_state(this, GST_STATE_NULL);
}

bool AudioPlayer::media(const std::string& uri)
{
    bool result = false;

    if (uri.empty())
    {
        throw std::runtime_error("invalid uri");
    }

    if (detail::change_if_diff(m_impl->m_uri, uri))
    {
        std::string path;
        auto type = detail::resolve_path(uri, path);
        switch (type)
        {
        case detail::SchemeType::filesystem:
        {
            result = createPipeline(std::string("file://") + path);
            break;
        }
        case detail::SchemeType::network:
        {
            result = createPipeline(uri);
            break;
        }
        default:
        {
            throw std::runtime_error("unsupported uri: " + uri);
        }
        }
    }

    return result;
}

bool AudioPlayer::volume(int volume)
{
    if (!m_impl->m_volume)
        return false;

    if (volume < 0)
        volume = 0;
    if (volume > 100)
        volume = 100;

    g_object_set(m_impl->m_volume, "volume", volume / 10.0, nullptr);
    on_state_changed.invoke();

    return true;
}

int AudioPlayer::volume() const
{
    if (!m_impl->m_volume)
        return 0;

    gdouble volume = 0;
    g_object_get(m_impl->m_volume, "volume", &volume, nullptr);
    return volume * 10.0;
}

bool AudioPlayer::mute(bool mute)
{
    if (!m_impl->m_volume)
        return false;

    g_object_set(m_impl->m_volume, "mute", mute, nullptr);
    on_state_changed.invoke();

    return true;
}

uint64_t AudioPlayer::position() const
{
    return nsec_to_sec(m_impl->m_position);
}

uint64_t AudioPlayer::duration() const
{
    return nsec_to_sec(m_impl->m_duration);
}

bool AudioPlayer::playing() const
{
    GstState state = GST_STATE_VOID_PENDING;

    if (m_impl->m_pipeline)
    {
        (void)gst_element_get_state(m_impl->m_pipeline, &state,
                                    nullptr, GST_CLOCK_TIME_NONE);
        return state == GST_STATE_PLAYING;
    }

    return false;
}

inline uint64_t sec_to_nsec(uint64_t s)
{
    return s * 1000000000ULL;
}

bool AudioPlayer::seek(uint64_t pos)
{
    if (m_impl->m_pipeline)
    {
        if (gst_element_seek(m_impl->m_pipeline, 1.0, GST_FORMAT_TIME,
                             GST_SEEK_FLAG_FLUSH,
                             GST_SEEK_TYPE_SET, sec_to_nsec(pos),
                             GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
        {
            return true;
        }
    }

    return false;
}

bool AudioPlayer::createPipeline(const std::string& uri)
{
    // Make sure we don't leave orphan references
    destroyPipeline();

    auto pipeline = fmt::format("uridecodebin uri={} expose-all-streams=false caps=audio/x-raw " \
                                "! audioconvert ! volume name=volume ! autoaudiosink", uri);

    SPDLOG_DEBUG(pipeline);

    GError* error = nullptr;
    m_impl->m_pipeline = gst_parse_launch(pipeline.c_str(), &error);
    if (!m_impl->m_pipeline)
    {
        spdlog::error("failed to create audio pipeline");
        on_error.invoke("failed to create audio pipeline");
        return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    m_impl->m_volume = gst_bin_get_by_name(GST_BIN(m_impl->m_pipeline), "volume");
    if (!m_impl->m_volume)
    {
        spdlog::error("failed to get volume element");
        on_error.invoke("failed to get volume element");
        return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(m_impl->m_pipeline));
    gst_bus_add_watch(bus, &bus_callback, m_impl.get());
    gst_object_unref(bus);

    m_impl->m_eventsource_id = g_timeout_add(900, &query_position, m_impl.get());

    if (!m_impl->m_gmain_loop)
    {
        m_impl->m_gmain_loop = g_main_loop_new(nullptr, FALSE);
        m_impl->m_gmain_thread = std::thread(g_main_loop_run, m_impl->m_gmain_loop);
    }

    return true;
}

}
}
