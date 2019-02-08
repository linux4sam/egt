/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/app.h"
#include "egt/audio.h"
#include "egt/utils.h"
#include "egt/video.h"
#include <asio.hpp>
#include <cstring>
#include <thread>

#ifdef HAVE_GSTREAMER

using namespace std;

#define SRC_NAME "srcAudio"
#define PROGRESS_NAME "progress"
#define VOLUME_NAME "volume"

namespace egt
{
inline namespace v1
{

gboolean AudioPlayer::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    auto _this = reinterpret_cast<AudioPlayer*>(data);

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
        DBG("GStreamer: Message CLOCK_PROVIDE");
        break;
    case GST_MESSAGE_CLOCK_LOST:
        DBG("GStreamer: Message CLOCK_LOST");
        break;
    case GST_MESSAGE_NEW_CLOCK:
        DBG("GStreamer: Message NEW_CLOCK");
        break;
    case GST_MESSAGE_EOS:
    {
        DBG("GStreamer: Message EOS");

        gst_element_seek(_this->m_audio_pipeline, 1.0, GST_FORMAT_TIME,
                         GST_SEEK_FLAG_FLUSH,
                         GST_SEEK_TYPE_SET, 0,
                         GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
        _this->pause();
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
            _this->m_duration = g_value_get_int64(vtotal);
            vcurrent = gst_structure_get_value(info, "current");
            _this->m_position = g_value_get_int64(vcurrent);

            main_app().event().io().post(std::bind(&AudioPlayer::invoke_handlers,
                                                   _this, eventid::PROPERTY_CHANGED));
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
{
    detail::init_gst_thread();
}

void AudioPlayer::destroyPipeline()
{
    if (m_audio_pipeline)
        (void)gst_element_set_state(m_audio_pipeline, GST_STATE_NULL);

    if (m_volume)
    {
        g_object_unref(m_volume);
        m_volume = NULL;
    }

    if (m_src)
    {
        g_object_unref(m_src);
        m_src = NULL;
    }

    if (m_audio_pipeline)
    {
        g_object_unref(m_audio_pipeline);
        m_audio_pipeline = NULL;
    }
}

AudioPlayer::~AudioPlayer()
{
    destroyPipeline();
}

bool AudioPlayer::play(bool mute, int volume)
{
    ignoreparam(mute);
    ignoreparam(volume);

    set_state(GST_STATE_PLAYING);
    return false;
}

bool AudioPlayer::unpause()
{
    set_state(GST_STATE_PLAYING);
    return false;
}

bool AudioPlayer::pause()
{
    set_state(GST_STATE_PAUSED);
    return false;
}

bool AudioPlayer::null()
{
    return set_state(GST_STATE_NULL);
}

bool AudioPlayer::set_media(const string& uri)
{
    m_filename = uri;

    destroyPipeline();
    createPipeline();
    g_object_set(m_src, "uri", (string("file://") + uri).c_str(), NULL);

    return true;
}

bool AudioPlayer::set_volume(int volume)
{
    if (!m_volume)
        return false;

    if (volume < 1)
        volume = 1;
    if (volume > 100)
        volume = 100;

    g_object_set(m_volume, "volume", volume / 100.0, NULL);
    invoke_handlers(eventid::PROPERTY_CHANGED);

    return true;
}

int AudioPlayer::get_volume() const
{
    if (!m_volume)
        return 0;

    gdouble volume = 0;
    g_object_get(m_volume, "volume", &volume, NULL);
    return volume * 100.0;
}

bool AudioPlayer::set_mute(bool mute)
{
    if (!m_volume)
        return false;

    g_object_set(m_volume, "mute", mute, NULL);
    invoke_handlers(eventid::PROPERTY_CHANGED);
    return true;
}

bool AudioPlayer::set_state(GstState state)
{
    GstStateChangeReturn ret;

    if (m_audio_pipeline)
    {
        ret = gst_element_set_state(m_audio_pipeline, state);
        if (GST_STATE_CHANGE_FAILURE == ret)
        {
            ERR("unable to set audio pipeline to " << state);
            return false;
        }

        main_app().event().io().post(std::bind(&AudioPlayer::invoke_handlers,
                                               this, eventid::PROPERTY_CHANGED));
    }
    else
    {
        return false;
    }

    return true;
}

bool AudioPlayer::playing() const
{
    GstState state = GST_STATE_VOID_PENDING;

    if (m_audio_pipeline)
    {
        (void)gst_element_get_state(m_audio_pipeline, &state,
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
    if (m_audio_pipeline)
    {
        gst_element_seek(m_audio_pipeline, 1.0, GST_FORMAT_TIME,
                         GST_SEEK_FLAG_FLUSH,
                         GST_SEEK_TYPE_SET, sec_to_nsec(pos),
                         GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
    }
}

/// @todo hard coded to second sound device
#define PIPE "uridecodebin expose-all-streams=false name=" SRC_NAME " caps=audio/x-raw " \
    "! progressreport silent=true do-query=true update-freq=1 format=time name=" PROGRESS_NAME " " \
    "! audioconvert ! volume name=" VOLUME_NAME " " \
    "! alsasink device=hw:1"

bool AudioPlayer::createPipeline()
{
    GError* error = NULL;
    GstBus* bus;
    //guint bus_watch_id;

    /* Make sure we don't leave orphan references */
    destroyPipeline();

    char buffer[2048];
    sprintf(buffer, PIPE);

    string pipe(buffer);
    DBG(pipe);

    m_audio_pipeline = gst_parse_launch(pipe.c_str(), &error);
    if (!m_audio_pipeline)
    {
        m_audio_pipeline = NULL;
        ERR("failed to create audio pipeline");
        return false;
    }

    m_src = gst_bin_get_by_name(GST_BIN(m_audio_pipeline), SRC_NAME);
    if (!m_src)
    {
        m_src = NULL;
        ERR("failed to get audio src element");
        return false;
    }

    m_volume = gst_bin_get_by_name(GST_BIN(m_audio_pipeline), VOLUME_NAME);
    if (!m_volume)
    {
        m_volume = NULL;
        ERR("failed to get volume element");
        return false;
    }

    bus = gst_pipeline_get_bus(GST_PIPELINE(m_audio_pipeline));
    /*bus_watch_id =*/ gst_bus_add_watch(bus, &bus_callback, this);
    gst_object_unref(bus);

    return true;
}

}
}

#endif
