/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "detail/egtlog.h"
#include "detail/multimedia/gstdecoderimpl.h"
#include "detail/multimedia/gstmeta.h"
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/app.h"
#include "egt/uri.h"
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

GstDecoderImpl::GstDecoderImpl(VideoWindow& iface, const Size& size)
    : m_interface(iface),
      m_size(size),
      m_audiodevice(detail::audio_device())
{
    detail::gstreamer_init();
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
            detail::error("unable to set the pipeline to the play state");
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
            detail::error("unable to set the pipeline to the pause state.");
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
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
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

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
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
                             // NOLINTNEXTLINE(google-readability-casting)
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
    return (m_audiodevice && m_audiotrack);
}

void GstDecoderImpl::destroyPipeline()
{
    if (m_pipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == ret)
        {
            detail::error("failed to set pipeline to GST_STATE_NULL");
        }
        g_object_unref(m_pipeline);
        m_pipeline = nullptr;

        if (m_bus)
        {
            g_source_remove(m_bus_watchid);
            gst_object_unref(m_bus);
            m_bus = nullptr;
        }

        if (m_eventsource_id > 0)
        {
            g_source_remove(m_eventsource_id);
            m_eventsource_id = 0;
        }

        if (m_gmain_loop)
        {
            /*
             * check loop is running to avoid race condition when stop is called too early
             */
            if (g_main_loop_is_running(m_gmain_loop))
            {
                //stop loop and wait
                g_main_loop_quit(m_gmain_loop);
            }
            m_gmain_thread.join();
            g_main_loop_unref(m_gmain_loop);
            m_gmain_loop = nullptr;
        }
    }
}

GstDecoderImpl::~GstDecoderImpl() = default;

gboolean GstDecoderImpl::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    auto impl = static_cast<GstDecoderImpl*>(data);

    EGTLOG_TRACE("gst message: {}", GST_MESSAGE_TYPE_NAME(message));

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gstreamer_message_parse(gst_message_parse_error, message, error, debug);
        if (error)
        {
            EGTLOG_DEBUG("gst error: {} {}",
                         error->message,
                         debug ? debug.get() : "");

            if (Application::check_instance())
            {
                asio::post(Application::instance().event().io(), [impl, error = std::move(error)]()
                {
                    impl->m_interface.on_error.invoke(error->message);
                });
            }
        }
        break;
    }
    case GST_MESSAGE_WARNING:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gstreamer_message_parse(gst_message_parse_warning, message, error, debug);
        if (error)
        {
            EGTLOG_DEBUG("gst warning: {} {}",
                         error->message,
                         debug ? debug.get() : "");
        }
        break;
    }
    case GST_MESSAGE_INFO:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gstreamer_message_parse(gst_message_parse_info, message, error, debug);
        if (error)
        {
            EGTLOG_DEBUG("gst info: {} {}",
                         error->message,
                         debug ? debug.get() : "");
        }
        break;
    }
    break;
    case GST_MESSAGE_EOS:
    {
        EGTLOG_DEBUG("loopback: {}", (impl->m_interface.loopback() ? "TRUE" : "FALSE"));
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
            if (Application::check_instance())
            {
                asio::post(Application::instance().event().io(), [impl]()
                {
                    impl->m_interface.on_eos.invoke();
                });
            }
        }
        break;
    }
    case GST_MESSAGE_STATE_CHANGED:
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        if (GST_MESSAGE_SRC(message) == GST_OBJECT(impl->m_pipeline))
        {
            GstState old_state, new_state, pending_state;
            gst_message_parse_state_changed(message, &old_state, &new_state, &pending_state);

            EGTLOG_DEBUG("state changed from {} to {}",
                         gst_element_state_get_name(old_state),
                         gst_element_state_get_name(new_state));

            gchar* dump_name = g_strdup_printf("egt.%s_%s",
                                               gst_element_state_get_name(old_state),
                                               gst_element_state_get_name(new_state));

            GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(impl->m_pipeline),
                                              GST_DEBUG_GRAPH_SHOW_ALL, dump_name);

            g_free(dump_name);

            if (impl->playing())
            {
                GstQuery* query = gst_query_new_seeking(GST_FORMAT_TIME);
                if (gst_element_query(impl->m_pipeline, query))
                {
                    gst_query_parse_seeking(query, nullptr, &impl->m_seek_enabled, &impl->m_start, &impl->m_duration);
                }
                gst_query_unref(query);
            }

            if (Application::check_instance())
            {
                asio::post(Application::instance().event().io(), [impl]()
                {
                    impl->m_interface.on_state_changed.invoke();
                });
            }
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
        detail::error("error creating discoverer instance: {} ", std::string(err1->message));
        m_interface.on_error.invoke("error creating discoverer instance: " + std::string(err1->message));
        g_clear_error(&err1);
        return false;
    }

    GError* err2 = nullptr;
    std::unique_ptr<GstDiscovererInfo, GstDeleter<void, g_object_unref>>
            info{gst_discoverer_discover_uri(discoverer.get(), m_uri.c_str(), &err2)};

    GstDiscovererResult result = gst_discoverer_info_get_result(info.get());
    EGTLOG_DEBUG("result: {} ", result);
    switch (result)
    {
    case GST_DISCOVERER_URI_INVALID:
    {
        detail::error("invalid URI: {}", m_uri);
        m_interface.on_error.invoke("invalid URI: " + m_uri);
        return false;
    }
    case GST_DISCOVERER_ERROR:
    {
        detail::error("error: {} ", std::string(err2->message));
        m_interface.on_error.invoke("error: " + std::string(err2->message));
        break;
    }
    case GST_DISCOVERER_TIMEOUT:
    {

        detail::error("gst discoverer timeout");
        m_interface.on_error.invoke("gst discoverer timeout");
        return false;
    }
    case GST_DISCOVERER_BUSY:
    {
        detail::error("gst discoverer busy");
        m_interface.on_error.invoke("gst discoverer busy");
        return false;
    }
    case GST_DISCOVERER_MISSING_PLUGINS:
    {
        const GstStructure* s = gst_discoverer_info_get_misc(info.get());
        GstStringHandle str{gst_structure_to_string(s)};
        m_interface.on_error.invoke(str.get());
        return false;
    }
    case GST_DISCOVERER_OK:
        EGTLOG_DEBUG("success");
        break;
    }

    std::unique_ptr<GstDiscovererStreamInfo, GstDeleter<void, g_object_unref>>
            sinfo{gst_discoverer_info_get_stream_info(info.get())};
    if (!sinfo)
    {
        detail::error("failed to get stream info");
        m_interface.on_error.invoke("failed to get stream info");
        return false;
    }

    std::unique_ptr<GList, GstDeleter<GList, gst_discoverer_stream_info_list_free>>
            streams{gst_discoverer_container_info_get_streams(GST_DISCOVERER_CONTAINER_INFO(sinfo.get()))};
    if (!streams)
    {
        detail::error("failed to get stream info list");
        m_interface.on_error.invoke("failed to get stream info list");
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
            EGTLOG_DEBUG("{} : {}", type, std::string(desc.get()));
        }
    }
}
#endif

void GstDecoderImpl::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);
    /*
     * its a Basic window copying buffer to Cairo surface.
     */
    if (m_videosample)
    {
        GstCaps* caps = gst_sample_get_caps(m_videosample);
        GstStructure* capsStruct = gst_caps_get_structure(caps, 0);
        int width = 0;
        int height = 0;
        gst_structure_get_int(capsStruct, "width", &width);
        gst_structure_get_int(capsStruct, "height", &height);

        gst_sample_ref(m_videosample);
        GstBuffer* buffer = gst_sample_get_buffer(m_videosample);
        if (buffer)
        {
            GstMapInfo map;
            if (gst_buffer_map(buffer, &map, GST_MAP_READ))
            {
                auto box = m_interface.box();
                auto surface = unique_cairo_surface_t(
                                   cairo_image_surface_create_for_data(map.data,
                                           CAIRO_FORMAT_RGB16_565,
                                           width,
                                           height,
                                           cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, width)));

                if (cairo_surface_status(surface.get()) == CAIRO_STATUS_SUCCESS)
                {
                    auto cr = painter.context().get();
                    if (width != box.width() || height != box.height())
                    {
                        double scalex = static_cast<double>(box.width()) / width;
                        double scaley = static_cast<double>(box.height()) / height;
                        cairo_scale(cr, scalex, scaley);
                    }
                    cairo_set_source_surface(cr, surface.get(), box.x(), box.y());
                    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
                    cairo_paint(cr);
                }

                m_position = GST_BUFFER_TIMESTAMP(buffer);
                gst_buffer_unmap(buffer, &map);
            }
        }
        gst_sample_unref(m_videosample);

    }
}

GstFlowReturn GstDecoderImpl::on_new_buffer(GstElement* elt, gpointer data)
{
    auto impl = static_cast<GstDecoderImpl*>(data);
    GstSample* sample;
    g_signal_emit_by_name(elt, "pull-sample", &sample);
    if (sample)
    {
#ifdef HAVE_LIBPLANES
        if (impl->m_interface.plane_window())
        {
            GstCaps* caps = gst_sample_get_caps(sample);
            GstStructure* capsStruct = gst_caps_get_structure(caps, 0);
            int width = 0;
            int height = 0;
            gst_structure_get_int(capsStruct, "width", &width);
            gst_structure_get_int(capsStruct, "height", &height);
            auto vs = egt::Size(width, height);
            auto b = impl->m_interface.content_area();
            /*
             * If scaling is requested, it's normal that the size of the
             * VideoWindow is different from the size of the video. Don't drop
             * the frame in this case.
             * Caution: checking only the scale may not enough to justifiy that
             * the window and video size are different and it won't lead to a
             * crash.
             */
            if (impl->m_interface.hscale() == 1.0 && impl->m_interface.vscale() == 1.0)
            {
                if (b.size() != vs)
                {
                    if (Application::check_instance())
                    {
                        asio::post(Application::instance().event().io(), [impl, vs, b]()
                        {
                            if (vs.width() < b.width() || vs.height() < b.height())
                                impl->resize(vs);
                            else
                                impl->resize(b.size());
                        });
                    }

                    // drop this frame and continue
                    gst_sample_unref(sample);
                    return GST_FLOW_OK;
                }
            }

            GstBuffer* buffer = gst_sample_get_buffer(sample);
            if (buffer)
            {
                GstMapInfo map;
                if (gst_buffer_map(buffer, &map, GST_MAP_READ))
                {
                    auto screen =
                        reinterpret_cast<detail::KMSOverlay*>(impl->m_interface.screen());
                    assert(screen);
                    memcpy(screen->raw(), map.data, map.size);
                    screen->schedule_flip();
                    impl->m_position = GST_BUFFER_TIMESTAMP(buffer);
                    gst_buffer_unmap(buffer, &map);
                }
            }
            gst_sample_unref(sample);
        }
        else
#endif
        {
            if (Application::check_instance())
            {
                asio::post(Application::instance().event().io(), [impl, sample]()
                {
                    if (impl->m_videosample)
                        gst_sample_unref(impl->m_videosample);

                    impl->m_videosample = sample;
                    impl->m_interface.damage();
                });
            }
        }
        return GST_FLOW_OK;
    }
    return GST_FLOW_ERROR;
}

std::string GstDecoderImpl::create_pipeline()
{
    std::string a_pipe;
    /*
     * GstURIDecodeBin caps are propagated to GstDecodeBin. Its caps must a be a
     * superset of the inner decoder element. So make this superset large enough
     * to contain the inner decoder's caps that have features.
     */
    std::string caps = " caps=video/x-raw(ANY)";
    if (m_audiodevice && m_audiotrack)
    {
        caps += ";audio/x-raw(ANY)";
        a_pipe = "! queue ! audioconvert ! volume name=volume ! autoaudiosink sync=false";
    }

    if (!m_interface.plane_window() && m_size.empty())
    {
        /*
         * If size is empty, then PlaneWindow is created with size(32,32) but not for
         * BasicWindow. so resizing BasicWindow to size(32,32).
         */
        m_interface.resize(Size(32, 32));
    }

    const auto format = m_interface.format();
    std::string video_format = detail::gstreamer_format(format);
    const auto video_caps_filter =
        fmt::format("caps=video/x-raw,width={},height={},format={}",
                    m_size.width(), m_size.height(), video_format);

    static constexpr auto pipeline =
        "uridecodebin uri={} expose-all-streams=false name=video " \
        " {} video. ! videoconvert ! videoscale ! capsfilter name=vcaps {} ! appsink name=appsink video. {} ";

    return fmt::format(pipeline, m_uri, caps, video_caps_filter, a_pipe);
}

/* This function takes a textual representation of a pipeline
 * and create an actual pipeline
 */
bool GstDecoderImpl::media(const std::string& uri)
{
    if (detail::change_if_diff(m_uri, uri))
    {
        /* Make sure we don't leave orphan references */
        destroyPipeline();

#ifdef HAVE_GSTREAMER_PBUTILS
        Uri u(m_uri);
        if (u.scheme() != "rtsp")
        {
            if (!start_discoverer())
            {
                detail::error("media file discoverer failed");
                return false;
            }
        }
#endif

        const auto buffer = create_pipeline();
        EGTLOG_DEBUG("{}", buffer);

        GError* error = nullptr;
        m_pipeline = gst_parse_launch(buffer.c_str(), &error);
        if (!m_pipeline)
        {
            if (error && error->message)
            {
                detail::error("{}", error->message);
                m_interface.on_error.invoke(error->message);
            }
            return false;
        }

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        m_vcapsfilter = gst_bin_get_by_name(GST_BIN(m_pipeline), "vcaps");
        if (!m_vcapsfilter)
        {
            detail::error("failed to get vcaps element");
            m_interface.on_error.invoke("failed to get vcaps element");
            return false;
        }

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "appsink");
        if (!m_appsink)
        {
            detail::error("failed to get app sink element");
            m_interface.on_error.invoke("failed to get app sink element");
            return false;
        }

        if (m_audiodevice && m_audiotrack)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
            m_volume = gst_bin_get_by_name(GST_BIN(m_pipeline), "volume");
            if (!m_volume)
            {
                detail::error("failed to get volume element");
                m_interface.on_error.invoke("failed to get volume element");
                return false;
            }
        }

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, nullptr);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        g_signal_connect(m_appsink, "new-sample", G_CALLBACK(on_new_buffer), this);

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
        m_bus_watchid = gst_bus_add_watch(m_bus, &bus_callback, this);

        g_timeout_add(5000, static_cast<GSourceFunc>(&post_position), this);

        if (!m_gmain_loop)
        {
            m_gmain_loop = g_main_loop_new(nullptr, false);
            m_gmain_thread = std::thread(g_main_loop_run, m_gmain_loop);
        }
    }
    return true;
}

void GstDecoderImpl::scale(float scalex, float scaley)
{
    m_interface.resize(Size(m_size.width() * scalex, m_size.height() * scaley));
}

void GstDecoderImpl::resize(const Size& size)
{
    if (size != m_size)
    {
        if (m_pipeline && m_vcapsfilter)
        {
            const auto format = m_interface.format();
            std::string vs = fmt::format("video/x-raw,width={},height={},format={}",
                                         size.width(), size.height(), detail::gstreamer_format(format));
            GstCaps* caps = gst_caps_from_string(vs.c_str());
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
            g_object_set(G_OBJECT(m_vcapsfilter), "caps", caps, NULL);
            EGTLOG_DEBUG("change gst videoscale element to {}", size);
            gst_caps_unref(caps);
        }

        if (m_size.empty())
        {
            m_size = size;
            EGTLOG_DEBUG("setting m_size to {} from {}", size, m_size);
        }
    }
}

gboolean GstDecoderImpl::post_position(gpointer data)
{
    auto impl = static_cast<GstDecoderImpl*>(data);

    if (Application::check_instance())
    {
        asio::post(Application::instance().event().io(), [impl]()
        {
            impl->m_interface.on_position_changed.invoke(impl->m_position);
        });
    }

    return true;
}

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
