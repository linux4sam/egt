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

#define SRC_NAME "srcAudio"
#define PROGRESS_NAME "progress"
#define VOLUME_NAME "volume"

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
    GstElement* m_src {nullptr};
    GstElement* m_volume {nullptr};
    gint64 m_position {0};
    gint64 m_duration {0};
    std::string m_uri;
    int m_volume_value {100};
    GMainLoop* m_gmainLoop{nullptr};
    std::thread m_gmainThread;
};

}

static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    detail::ignoreparam(bus);

    auto impl = reinterpret_cast<detail::AudioPlayerImpl*>(data);

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
    case GST_MESSAGE_ELEMENT:
    {
        const GstStructure* info = gst_message_get_structure(message);
        if (gst_structure_has_name(info, PROGRESS_NAME))
        {
            const GValue* vtotal = gst_structure_get_value(info, "total");
            const auto total = g_value_get_int64(vtotal);
            const GValue* vcurrent = gst_structure_get_value(info, "current");
            const auto current = g_value_get_int64(vcurrent);

            if (Application::check_instance())
            {
                asio::post(Application::instance().event().io(), [impl, current, total]()
                {
                    impl->m_duration = total;

                    if (detail::change_if_diff<>(impl->m_position, current))
                    {
                        impl->player.on_position_changed.invoke();
                    }
                });
            }
        }
        break;
    }
    case GST_MESSAGE_STATE_CHANGED:
    {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed(message, &old_state, &new_state, &pending_state);
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
    : m_impl(new detail::AudioPlayerImpl(*this))
{
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
    media(uri);
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

    if (m_impl->m_src)
    {
        g_object_unref(m_impl->m_src);
        m_impl->m_src = nullptr;
    }

    if (m_impl->m_pipeline)
    {
        g_object_unref(m_impl->m_pipeline);
        m_impl->m_pipeline = nullptr;
    }

    if (m_impl->m_gmainLoop)
    {
        // check loop is running to avoid race condition when stop is called too early
        if (g_main_loop_is_running(m_impl->m_gmainLoop))
        {
            //stop loop and wait
            g_main_loop_quit(m_impl->m_gmainLoop);
        }
        m_impl->m_gmainThread.join();
        g_main_loop_unref(m_impl->m_gmainLoop);
        m_impl->m_gmainLoop = nullptr;
    }
}

AudioPlayer::~AudioPlayer()
{
    destroyPipeline();
}

bool AudioPlayer::play()
{
    return m_impl->change_state(this, GST_STATE_PLAYING);
}

bool AudioPlayer::unpause()
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

    if (detail::change_if_diff(m_impl->m_uri, uri))
    {
        destroyPipeline();

        std::string path;
        auto type = detail::resolve_path(uri, path);

        switch (type)
        {
        case detail::SchemeType::filesystem:
        {
            result = createPipeline();
            g_object_set(m_impl->m_src,
                         "uri",
                         (std::string("file://") + path).c_str(),
                         nullptr);
            break;
        }
        case detail::SchemeType::network:
        {
            result = createPipeline();
            g_object_set(m_impl->m_src,
                         "uri",
                         uri.c_str(),
                         nullptr);
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
    return m_impl->m_position;
}

uint64_t AudioPlayer::duration() const
{
    return m_impl->m_duration;
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

#define PIPE "uridecodebin expose-all-streams=false name=" SRC_NAME " caps=audio/x-raw " \
    "! progressreport silent=true do-query=true update-freq=1 format=time name=" PROGRESS_NAME " " \
    "! audioconvert ! volume name=" VOLUME_NAME " " \
    "! autoaudiosink"

bool AudioPlayer::createPipeline()
{
    GError* error = nullptr;
    GstBus* bus;
    //guint bus_watch_id;

    // Make sure we don't leave orphan references
    destroyPipeline();

    std::string pipe(PIPE);
    SPDLOG_DEBUG(pipe);

    m_impl->m_pipeline = gst_parse_launch(pipe.c_str(), &error);
    if (!m_impl->m_pipeline)
    {
        spdlog::error("failed to create audio pipeline");
        on_error.invoke("failed to create audio pipeline");
        return false;
    }

    m_impl->m_src = gst_bin_get_by_name(GST_BIN(m_impl->m_pipeline), SRC_NAME);
    if (!m_impl->m_src)
    {
        spdlog::error("failed to get audio src element");
        on_error.invoke("failed to get audio src element");
        return false;
    }

    m_impl->m_volume = gst_bin_get_by_name(GST_BIN(m_impl->m_pipeline), VOLUME_NAME);
    if (!m_impl->m_volume)
    {
        spdlog::error("failed to get volume element");
        on_error.invoke("failed to get volume element");
        return false;
    }

    bus = gst_pipeline_get_bus(GST_PIPELINE(m_impl->m_pipeline));
    /*bus_watch_id =*/ gst_bus_add_watch(bus, &bus_callback, m_impl.get());
    gst_object_unref(bus);

    if (!m_impl->m_gmainLoop)
    {
        m_impl->m_gmainLoop = g_main_loop_new(nullptr, FALSE);
        m_impl->m_gmainThread = std::thread(g_main_loop_run, m_impl->m_gmainLoop);
    }

    return true;
}

}
}
