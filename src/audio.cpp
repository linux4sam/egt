/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/app.h"
#include "egt/audio.h"
#include "egt/utils.h"
#include "egt/video.h"
#include <exception>
#include <gst/gst.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <thread>

using namespace std;

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
    bool set_state(AudioPlayer* player, GstState state)
    {
        GstStateChangeReturn ret;

        if (m_audio_pipeline)
        {
            ret = gst_element_set_state(m_audio_pipeline, state);
            if (GST_STATE_CHANGE_FAILURE == ret)
            {
                spdlog::error("unable to set audio pipeline to {}", state);
                return false;
            }

            asio::post(Application::instance().event().io(), [player]()
            {
                Event event(eventid::property_changed);
                player->invoke_handlers(event);
            });
        }
        else
        {
            return false;
        }

        return true;
    }


    AudioPlayer& player;
    GstElement* m_audio_pipeline {nullptr};
    GstElement* m_src {nullptr};
    GstElement* m_volume {nullptr};
    gint64 m_position {0};
    gint64 m_duration {0};
    std::string m_filename;
    int m_volume_value {100};
    GMainLoop* m_gmainLoop{nullptr};
    std::thread m_gmainThread;
};

}

static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    auto impl = reinterpret_cast<detail::AudioPlayerImpl*>(data);

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        GError* error;
        gchar* debug;

        gst_message_parse_error(message, &error, &debug);
        cout << "error: " << error->message << endl;
        g_error_free(error);
        g_free(debug);
        break;
    }
    case GST_MESSAGE_WARNING:
        break;
    case GST_MESSAGE_INFO:
        break;
    case GST_MESSAGE_CLOCK_PROVIDE:
        SPDLOG_DEBUG("GStreamer: Message CLOCK_PROVIDE");
        break;
    case GST_MESSAGE_CLOCK_LOST:
        SPDLOG_DEBUG("GStreamer: Message CLOCK_LOST");
        break;
    case GST_MESSAGE_NEW_CLOCK:
        SPDLOG_DEBUG("GStreamer: Message NEW_CLOCK");
        break;
    case GST_MESSAGE_EOS:
    {
        SPDLOG_DEBUG("GStreamer: Message EOS");

        gst_element_seek(impl->m_audio_pipeline, 1.0, GST_FORMAT_TIME,
                         GST_SEEK_FLAG_FLUSH,
                         GST_SEEK_TYPE_SET, 0,
                         GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
        impl->player.pause();
        break;
    }
    case GST_MESSAGE_ELEMENT:
    {
        const GstStructure* info = gst_message_get_structure(message);
        if (gst_structure_has_name(info, PROGRESS_NAME))
        {
            const GValue* vcurrent;
            const GValue* vtotal;

            vtotal = gst_structure_get_value(info, "total");
            impl->m_duration = g_value_get_int64(vtotal);
            vcurrent = gst_structure_get_value(info, "current");
            impl->m_position = g_value_get_int64(vcurrent);

            asio::post(Application::instance().event().io(), [impl]()
            {
                Event event(eventid::property_changed);
                impl->player.invoke_handlers(event);
            });
        }
        break;
    }
    default:
        /* unhandled message */
        break;
    }

    /* we want to be notified again the next time there is a message
     * on the bus, so returning TRUE (FALSE means we want to stop watching
     * for messages on the bus and our callback should not be called again)
     */
    return TRUE;
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

    m_impl->m_gmainLoop = g_main_loop_new(NULL, FALSE);
    m_impl->m_gmainThread = std::thread(g_main_loop_run, m_impl->m_gmainLoop);
}

void AudioPlayer::destroyPipeline()
{
    if (m_impl->m_audio_pipeline)
        (void)gst_element_set_state(m_impl->m_audio_pipeline, GST_STATE_NULL);

    if (m_impl->m_volume)
    {
        g_object_unref(m_impl->m_volume);
        m_impl->m_volume = NULL;
    }

    if (m_impl->m_src)
    {
        g_object_unref(m_impl->m_src);
        m_impl->m_src = NULL;
    }

    if (m_impl->m_audio_pipeline)
    {
        g_object_unref(m_impl->m_audio_pipeline);
        m_impl->m_audio_pipeline = NULL;
    }
}

AudioPlayer::~AudioPlayer()
{
    destroyPipeline();

    if (m_impl->m_gmainLoop)
    {
        /*
         * check loop is running to avoid race condition when stop is called too early
         */
        if (g_main_loop_is_running(m_impl->m_gmainLoop))
        {
            //stop loop and wait
            g_main_loop_quit(m_impl->m_gmainLoop);
        }
        m_impl->m_gmainThread.join();
        g_main_loop_unref(m_impl->m_gmainLoop);
    }
}

bool AudioPlayer::play(bool mute, int volume)
{
    ignoreparam(mute);
    ignoreparam(volume);

    m_impl->set_state(this, GST_STATE_PLAYING);
    return false;
}

bool AudioPlayer::unpause()
{
    m_impl->set_state(this, GST_STATE_PLAYING);
    return false;
}

bool AudioPlayer::pause()
{
    m_impl->set_state(this, GST_STATE_PAUSED);
    return false;
}

bool AudioPlayer::null()
{
    return m_impl->set_state(this, GST_STATE_NULL);
}

bool AudioPlayer::set_media(const string& uri)
{
    m_impl->m_filename = uri;

    destroyPipeline();
    createPipeline();
    g_object_set(m_impl->m_src, "uri", (string("file://") + uri).c_str(), NULL);

    return true;
}

bool AudioPlayer::set_volume(int volume)
{
    if (!m_impl->m_volume)
        return false;

    if (volume < 1)
        volume = 1;
    if (volume > 100)
        volume = 100;

    g_object_set(m_impl->m_volume, "volume", volume / 100.0, NULL);
    invoke_handlers(eventid::property_changed);

    return true;
}

int AudioPlayer::get_volume() const
{
    if (!m_impl->m_volume)
        return 0;

    gdouble volume = 0;
    g_object_get(m_impl->m_volume, "volume", &volume, NULL);
    return volume * 100.0;
}

bool AudioPlayer::set_mute(bool mute)
{
    if (!m_impl->m_volume)
        return false;

    g_object_set(m_impl->m_volume, "mute", mute, NULL);
    invoke_handlers(eventid::property_changed);
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

    if (m_impl->m_audio_pipeline)
    {
        (void)gst_element_get_state(m_impl->m_audio_pipeline, &state,
                                    NULL, GST_CLOCK_TIME_NONE);
        return state == GST_STATE_PLAYING;
    }

    return false;
}

inline uint64_t sec_to_nsec(uint64_t s)
{
    return s * 1000000000ULL;
}

void AudioPlayer::seek(uint64_t pos)
{
    if (m_impl->m_audio_pipeline)
    {
        gst_element_seek(m_impl->m_audio_pipeline, 1.0, GST_FORMAT_TIME,
                         GST_SEEK_FLAG_FLUSH,
                         GST_SEEK_TYPE_SET, sec_to_nsec(pos),
                         GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
    }
}

#define PIPE "uridecodebin expose-all-streams=false name=" SRC_NAME " caps=audio/x-raw " \
    "! progressreport silent=true do-query=true update-freq=1 format=time name=" PROGRESS_NAME " " \
    "! audioconvert ! volume name=" VOLUME_NAME " " \
    "! autoaudiosink"

bool AudioPlayer::createPipeline()
{
    GError* error = NULL;
    GstBus* bus;
    //guint bus_watch_id;

    /* Make sure we don't leave orphan references */
    destroyPipeline();

    string pipe(PIPE);
    SPDLOG_DEBUG(pipe);

    m_impl->m_audio_pipeline = gst_parse_launch(pipe.c_str(), &error);
    if (!m_impl->m_audio_pipeline)
    {
        spdlog::error("failed to create audio pipeline");
        return false;
    }

    m_impl->m_src = gst_bin_get_by_name(GST_BIN(m_impl->m_audio_pipeline), SRC_NAME);
    if (!m_impl->m_src)
    {
        spdlog::error("failed to get audio src element");
        return false;
    }

    m_impl->m_volume = gst_bin_get_by_name(GST_BIN(m_impl->m_audio_pipeline), VOLUME_NAME);
    if (!m_impl->m_volume)
    {
        spdlog::error("failed to get volume element");
        return false;
    }

    bus = gst_pipeline_get_bus(GST_PIPELINE(m_impl->m_audio_pipeline));
    /*bus_watch_id =*/ gst_bus_add_watch(bus, &bus_callback, m_impl.get());
    gst_object_unref(bus);

    return true;
}

}
}
