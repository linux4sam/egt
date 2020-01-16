/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "detail/video/gstdecoderimpl.h"
#include "detail/video/gstmeta.h"
#include <egt/app.h>
#include <exception>
#include <fstream>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

GstDecoderImpl::GstDecoderImpl(VideoWindow& interface, const Size& size)
    : m_interface(interface),
      m_size(size),
      m_audiodevice(detail::audio_device())
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

    m_gmainLoop = g_main_loop_new(nullptr, FALSE);
    m_gmainThread = std::thread(g_main_loop_run, m_gmainLoop);
}

bool GstDecoderImpl::playing() const
{
    GstState state = GST_STATE_VOID_PENDING;

    if (m_pipeline)
    {
        (void)gst_element_get_state(m_pipeline, &state, nullptr,
                                    GST_CLOCK_TIME_NONE);
        return state == GST_STATE_PLAYING;
    }
    return false;
}

bool GstDecoderImpl::play()
{
    if (m_pipeline && !playing())
    {
        auto ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            spdlog::error("unable to set the pipeline to the play state");
            destroyPipeline();
            return false;
        }
    }
    return true;
}

bool GstDecoderImpl::pause()
{
    if (m_pipeline)
    {
        auto ret = gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
        if (ret == GST_STATE_CHANGE_FAILURE)
        {
            spdlog::error("unable to set the pipeline to the pause state.");
            destroyPipeline();
            return false;
        }
    }
    return true;
}

int GstDecoderImpl::volume() const
{
    if (!m_volume)
        return 0;

    gdouble volume = 0;
    g_object_get(m_volume, "volume", &volume, nullptr);
    return volume * 10.0;
}

bool GstDecoderImpl::volume(int volume)
{
    if (!m_volume)
        return false;

    if (volume < 0)
        volume = 0;
    else if (volume > 100)
        volume = 100;

    g_object_set(m_volume, "volume", volume / 10.0, nullptr);
    return true;
}

bool GstDecoderImpl::seek(int64_t time)
{
    /* previous seek still in progress */
    if (m_seek_enabled && !m_seekdone)
    {
        m_seekdone = true;
        if (gst_element_seek(m_pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                             GST_SEEK_TYPE_SET, (gint64) time,
                             GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
        {
            m_seekdone = false;
            gst_element_get_state(m_pipeline, nullptr, nullptr, GST_CLOCK_TIME_NONE);
            gst_element_query_position(m_pipeline, GST_FORMAT_TIME, &m_position);
            return true;
        }
        m_seekdone = false;
    }
    return false;
}

std::string GstDecoderImpl::error_message() const
{
    return m_err_message;
}

int64_t GstDecoderImpl::duration() const
{
    return m_duration;
}

int64_t GstDecoderImpl::position() const
{
    return m_position;
}

bool GstDecoderImpl::has_audio() const
{
    if (m_audiodevice && m_audiotrack)
        return true;

    return false;
}

void GstDecoderImpl::destroyPipeline()
{
    if (m_pipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == ret)
        {
            spdlog::error("failed to set pipeline to GST_STATE_NULL");
        }
        g_object_unref(m_pipeline);
        m_pipeline = nullptr;
    }

    if (m_bus)
    {
        g_source_remove(m_bus_watchid);
        gst_object_unref(m_bus);
        m_bus = nullptr;
    }
}

GstDecoderImpl::~GstDecoderImpl()
{
    if (m_gmainLoop)
    {
        /*
         * check loop is running to avoid race condition when stop is called too early
         */
        if (g_main_loop_is_running(m_gmainLoop))
        {
            //stop loop and wait
            g_main_loop_quit(m_gmainLoop);
        }
        m_gmainThread.join();
        g_main_loop_unref(m_gmainLoop);
    }
}

gboolean GstDecoderImpl::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    auto impl = reinterpret_cast<GstDecoderImpl*>(data);

    SPDLOG_TRACE("gst message: {}", GST_MESSAGE_TYPE_NAME(message));

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gst_message_parse(gst_message_parse_error, message, error, debug);
        if (error)
        {
            SPDLOG_DEBUG("gst error: {} {}",
                         error->message,
                         debug ? debug.get() : "");

            std::string error_message = error->message;
            asio::post(Application::instance().event().io(), [impl, error_message]()
            {
                impl->m_err_message = error_message;
                impl->m_interface.on_error.invoke();
            });
        }
        break;
    }
    case GST_MESSAGE_WARNING:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gst_message_parse(gst_message_parse_warning, message, error, debug);
        if (error)
        {
            SPDLOG_DEBUG("gst warning: {} {}",
                         error->message,
                         debug ? debug.get() : "");
        }
        break;
    }
    case GST_MESSAGE_INFO:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gst_message_parse(gst_message_parse_info, message, error, debug);
        if (error)
        {
            SPDLOG_DEBUG("gst info: {} {}",
                         error->message,
                         debug ? debug.get() : "");
        }
        break;
    }
    break;
    case GST_MESSAGE_EOS:
    {
        SPDLOG_DEBUG("loopback: {}", (impl->m_interface.loopback() ? "TRUE" : "FALSE"));
        if (impl->m_interface.loopback())
        {
            gst_element_seek(impl->m_pipeline, 1.0, GST_FORMAT_TIME,
                             GST_SEEK_FLAG_FLUSH,
                             GST_SEEK_TYPE_SET, 0,
                             GST_SEEK_TYPE_NONE, -1);

            gst_element_set_state(impl->m_pipeline, GST_STATE_PLAYING);
        }
        else
        {
            asio::post(Application::instance().event().io(), [impl]()
            {
                impl->m_interface.on_eos.invoke();
            });
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

            if (impl->playing())
            {
                GstQuery* query = gst_query_new_seeking(GST_FORMAT_TIME);
                if (gst_element_query(impl->m_pipeline, query))
                {
                    gst_query_parse_seeking(query, nullptr, &impl->m_seek_enabled, &impl->m_start, &impl->m_duration);
                }
                gst_query_unref(query);
            }

            asio::post(Application::instance().event().io(), [impl]()
            {
                impl->m_interface.on_state_changed.invoke();
            });
        }
        break;
    }
    default:
        break;
    }

    /* we want to be notified again next time if there is a message
     * on the bus, so returning true (false means we want to stop watching
     * for messages on the bus and our callback should not be called again)
     */
    return true;
}

#ifdef HAVE_GSTREAMER_PBUTILS
bool GstDecoderImpl::start_discoverer()
{
    GError* err1 = nullptr;
    std::unique_ptr<GstDiscoverer, GstDeleter<void, g_object_unref>>
            discoverer{gst_discoverer_new(5 * GST_SECOND, &err1)};
    if (!discoverer)
    {
        m_err_message = "error creating discoverer instance: " + std::string(err1->message);
        g_clear_error(&err1);
        return false;
    }

    GError* err2 = nullptr;
    std::unique_ptr<GstDiscovererInfo, GstDeleter<void, g_object_unref>>
            info{gst_discoverer_discover_uri(discoverer.get(), ("file://" + m_uri).c_str(), &err2)};

    GstDiscovererResult result = gst_discoverer_info_get_result(info.get());
    SPDLOG_DEBUG("result: {} ", result);
    switch (result)
    {
    case GST_DISCOVERER_URI_INVALID:
    {
        m_err_message = "invalid URI: " + m_uri;
        return false;
    }
    case GST_DISCOVERER_ERROR:
    {
        m_err_message = "error: " + std::string(err2->message);
        break;
    }
    case GST_DISCOVERER_TIMEOUT:
    {
        m_err_message = "timeout";
        return false;
    }
    case GST_DISCOVERER_BUSY:
    {
        m_err_message = "busy";
        return false;
    }
    case GST_DISCOVERER_MISSING_PLUGINS:
    {
        const GstStructure* s = gst_discoverer_info_get_misc(info.get());
        GstStringHandle str{gst_structure_to_string(s)};
        m_err_message = str.get();
        return false;
    }
    case GST_DISCOVERER_OK:
        SPDLOG_DEBUG("success");
        break;
    }

    std::unique_ptr<GstDiscovererStreamInfo, GstDeleter<void, g_object_unref>>
            sinfo{gst_discoverer_info_get_stream_info(info.get())};
    if (!sinfo)
    {
        m_err_message = "failed to get stream info";
        return false;
    }

    std::unique_ptr<GList, GstDeleter<GList, gst_discoverer_stream_info_list_free>>
            streams{gst_discoverer_container_info_get_streams(GST_DISCOVERER_CONTAINER_INFO(sinfo.get()))};
    if (!streams)
    {
        m_err_message = "failed to get stream info list";
        return false;
    }

    GList* cinfo = streams.get();
    while (cinfo)
    {
        GstDiscovererStreamInfo* tmpinf = (GstDiscovererStreamInfo*)cinfo->data;
        get_stream_info(tmpinf);
        cinfo = cinfo->next;
    }
    return true;
}

/* Print information regarding a stream */
void GstDecoderImpl::get_stream_info(GstDiscovererStreamInfo* info)
{
    std::unique_ptr<GstCaps, GstDeleter<GstCaps, gst_caps_unref>>
            caps{gst_discoverer_stream_info_get_caps(info)};
    if (caps)
    {
        std::shared_ptr<gchar> desc;
        if (gst_caps_is_fixed(caps.get()))
            desc.reset(gst_pb_utils_get_codec_description(caps.get()), g_free);
        else
            desc.reset(gst_caps_to_string(caps.get()), g_free);

        if (desc)
        {
            std::string type = std::string(gst_discoverer_stream_info_get_stream_type_nick(info));
            if (!type.compare("video"))
            {
                m_vcodec = desc.get();
            }
            else if (!type.compare("audio"))
            {
                m_acodec = desc.get();
                m_audiotrack = true;
            }
            else if (!type.compare("container"))
            {
                m_container = desc.get();
            }
            SPDLOG_DEBUG("{} : {}", type, std::string(desc.get()));
        }
    }
}
#endif

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
