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

    m_gmain_loop = g_main_loop_new(nullptr, false);
    m_gmain_thread = std::thread(g_main_loop_run, m_gmain_loop);

    m_device_monitor = gst_device_monitor_new();

    GstCaps* caps = gst_caps_new_empty_simple("video/x-raw");
    gst_device_monitor_add_filter(m_device_monitor, "Video/Source", caps);
    gst_caps_unref(caps);

    GList* devlist = gst_device_monitor_get_devices(m_device_monitor);
    for (GList* i = g_list_first(devlist); i; i = g_list_next(i))
    {
        auto device = static_cast<GstDevice*>(i->data);
        if (!device)
            continue;

        const std::string devnode = gstreamer_get_device_path(device);

        if (std::find(m_devices.begin(), m_devices.end(), devnode) == m_devices.end())
            m_devices.push_back(devnode);
    }
    g_list_free(devlist);

    GstBus* bus = gst_device_monitor_get_bus(m_device_monitor);
    gst_bus_add_watch(bus, &device_monitor_bus_callback, this);

    gst_device_monitor_start(m_device_monitor);
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
    if (!m_pipeline)
    {
        const auto buffer = create_pipeline_desc();
        EGTLOG_DEBUG("{}", buffer);

        if (!create_pipeline(buffer))
            return false;
    }

    if (!playing())
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
    }
}

GstDecoderImpl::~GstDecoderImpl()
{
    destroyPipeline();

    if (m_device_monitor)
    {
        GstBus* bus = gst_device_monitor_get_bus(m_device_monitor);
        gst_bus_remove_watch(bus);
        gst_device_monitor_stop(m_device_monitor);
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
    }
}

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
                    impl->on_error.invoke(error->message);
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
                    impl->on_eos.invoke();
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
                    impl->on_state_changed.invoke();
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
        on_error.invoke("error creating discoverer instance: " + std::string(err1->message));
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
        on_error.invoke("invalid URI: " + m_uri);
        return false;
    }
    case GST_DISCOVERER_ERROR:
    {
        detail::error("error: {} ", std::string(err2->message));
        on_error.invoke("error: " + std::string(err2->message));
        break;
    }
    case GST_DISCOVERER_TIMEOUT:
    {

        detail::error("gst discoverer timeout");
        on_error.invoke("gst discoverer timeout");
        return false;
    }
    case GST_DISCOVERER_BUSY:
    {
        detail::error("gst discoverer busy");
        on_error.invoke("gst discoverer busy");
        return false;
    }
    case GST_DISCOVERER_MISSING_PLUGINS:
    {
        const GstStructure* s = gst_discoverer_info_get_misc(info.get());
        GstStringHandle str{gst_structure_to_string(s)};
        on_error.invoke(str.get());
        return false;
    }
    case GST_DISCOVERER_OK:
        EGTLOG_DEBUG("success");
        break;
    }

    GList* audio_streams = gst_discoverer_info_get_audio_streams(info.get());
    if (audio_streams)
    {
        m_audiotrack = true;
        gst_discoverer_stream_info_list_free(audio_streams);
    }

    return true;
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

std::string GstDecoderImpl::create_pipeline_desc()
{
    if (!m_custom_pipeline_desc.empty())
        return m_custom_pipeline_desc;

    auto src_plugin = std::string{};
    auto src_plugin_properties = std::string{};
    /*
     * The uridecodebin deals with files not devices. If we detect that users
     * provide a video device path, let's switch to the v4l2src plugin.
     */
    if (m_devnode.empty())
    {
        /*
         * GstURIDecodeBin caps are propagated to GstDecodeBin. Its caps must a be a
         * superset of the inner decoder element. So make this superset large enough
         * to contain the inner decoder's caps that have features.
         */
        src_plugin = "uridecodebin";
        src_plugin_properties = "uri=" + m_uri + " name=video caps=video/x-raw(ANY)";
        if (has_audio())
            src_plugin_properties += ";audio/x-raw(ANY)";

    }
    else
    {
        src_plugin = "v4l2src";
        src_plugin_properties = "device=" + m_devnode + " name=video";
    }

    const auto src = src_plugin + " " + src_plugin_properties;

    const auto audio = (m_audiodevice && m_audiotrack)
                       ? "video. ! queue ! audioconvert ! volume name=volume ! autoaudiosink sync=false"
                       : "";

    if (!m_interface.plane_window() && m_size.empty())
    {
        /*
         * If size is empty, then PlaneWindow is created with size(32,32) but not for
         * BasicWindow. so resizing BasicWindow to size(32,32).
         */
        m_interface.resize(Size(32, 32));
    }

    const auto format = m_interface.format();
    const auto video_caps_filter =
        fmt::format("caps=video/x-raw,width={},height={},format={}",
                    m_size.width(), m_size.height(), detail::gstreamer_format(format));

    static constexpr auto pipeline =
        "{} video. ! videoconvert ! videoscale ! capsfilter name=vcaps {} ! appsink name=appsink {} ";

    return fmt::format(pipeline, src, video_caps_filter, audio);
}

bool GstDecoderImpl::create_pipeline(const std::string& pipeline_desc)
{
    GError* error = nullptr;
    m_pipeline = gst_parse_launch(pipeline_desc.c_str(), &error);
    if (!m_pipeline)
    {
        if (error && error->message)
        {
            detail::error("{}", error->message);
            on_error.invoke(error->message);
        }
        return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    m_vcapsfilter = gst_bin_get_by_name(GST_BIN(m_pipeline), "vcaps");
    if (!m_vcapsfilter)
    {
        detail::error("failed to get vcaps element");
        on_error.invoke("failed to get vcaps element");
        destroyPipeline();
        return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "appsink");
    if (!m_appsink)
    {
        detail::error("failed to get app sink element");
        on_error.invoke("failed to get app sink element");
        destroyPipeline();
        return false;
    }

    if (m_audiodevice && m_audiotrack)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        m_volume = gst_bin_get_by_name(GST_BIN(m_pipeline), "volume");
        if (!m_volume)
        {
            detail::error("failed to get volume element");
            on_error.invoke("failed to get volume element");
            destroyPipeline();
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

    return true;
}

bool GstDecoderImpl::media(const std::string& uri)
{
    if (detail::change_if_diff(m_uri, uri))
    {
        /* Make sure we don't leave orphan references */
        destroyPipeline();

        const auto pos = m_uri.find("/dev/video");
        if (pos == std::string::npos)
        {
            m_devnode = "";
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
        }
        else
        {
            /*
             * Extract the device path. In the case of a video device, for
             * the pipeline description, we just want the device path and not
             * an uri.
             */
            m_devnode = std::string{m_uri.begin() + pos, m_uri.end()};
        }
    }

    return true;
}

void GstDecoderImpl::custom_pipeline(const std::string& pipeline_description)
{
    m_custom_pipeline_desc = pipeline_description;
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
            impl->on_position_changed.invoke(impl->m_position);
        });
    }

    return true;
}

bool GstDecoderImpl::start()
{
    get_camera_device_caps();

    Rect box;
    /*
     * At this stage, the interface m_box may no longer represent the original
     * size requested by the user if scaling occured. So compute the content
     * area based on the m_user_requested_box. If the user requested box is
     * empty, let's consider the user relies on automatic layout, then use the
     * window box.
     */
    if (!m_interface.user_requested_box().empty())
    {
        box = m_interface.user_requested_box();
        auto m = m_interface.moat();
        box += Point(m, m);
        box -= Size(2. * m, 2. * m);
        if (box.empty())
            box = Rect(m_interface.point(), m_interface.size());
    }
    else
    {
        box = m_interface.content_area();
    }
    EGTLOG_DEBUG("box = {}", box);

    /*
     * if user constructs a default constructor, then size of
     * the camerawindow is zero for BasicWindow and 32x32 for
     * plane window. due to which pipeline initialization fails
     * incase of BasicWindow. as a fix resize the camerawindow
     * to 32x32.
     */
    if ((box.width() < 32) && (box.height() < 32))
    {
        m_interface.resize(Size(32, 32));
        m_size = Size(32, 32);
        box = m_interface.content_area();
    }

    /*
     * Here we try to match camera resolution with camerawindow size
     * and add scaling to pipeline if size does not match.
     * note: adding scaling to may effects performance and this way
     * now users can set any size for camera window.
     */
    auto w = box.width();
    auto h = box.height();
    if (!m_resolutions.empty())
    {
        auto index = std::distance(m_resolutions.begin(),
                                   std::lower_bound(m_resolutions.begin(), m_resolutions.end(),
                                           std::make_tuple(box.width(), box.height())));

        w = std::get<0>(m_resolutions.at(index));
        h = std::get<1>(m_resolutions.at(index));

        EGTLOG_DEBUG("closest match of camerawindow : {} is {} ", box.size(), Size(w, h));
    }

    std::string vscale;
    if ((w != box.width()) || (h != box.height()))
    {
        vscale = fmt::format(" videoscale ! video/x-raw,width={},height={} !", box.width(), box.height());
        EGTLOG_DEBUG("scaling video: {} to {} ", Size(w, h), box.size());
    }

    const auto gst_format = detail::gstreamer_format(m_interface.format());
    EGTLOG_DEBUG("format: {}  ", gst_format);

    static constexpr auto appsink_pipe =
        "v4l2src device={} ! videoconvert ! video/x-raw,width={},height={},format={} ! {} " \
        "appsink name=appsink async=false enable-last-sample=false sync=true";

    const std::string pipe = fmt::format(appsink_pipe, m_devnode, w, h, gst_format, vscale);

    EGTLOG_DEBUG(pipe);

    /* Make sure we don't leave orphan references */
    stop();

    GError* error = nullptr;
    m_pipeline = gst_parse_launch(pipe.c_str(), &error);
    if (!m_pipeline)
    {
        on_error.invoke(fmt::format("failed to create pipeline: {}", error->message));
        return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "appsink");
    if (!m_appsink)
    {
        on_error.invoke("failed to get app sink element");
        return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, nullptr);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    g_signal_connect(m_appsink, "new-sample", G_CALLBACK(on_new_buffer), this);

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(bus, &bus_callback, this);
    gst_object_unref(bus);

    /*
     * GStreamer documentation states about GstParse that these functions take
     * several measures to create somewhat dynamic pipelines. Due to that such
     * pipelines are not always reusable (set the state to NULL and back to
     * PLAYING).
     */
    int ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        on_error.invoke("failed to set pipeline to null state");
        stop();
        return false;
    }

    ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        on_error.invoke("failed to set pipeline to play state");
        stop();
        return false;
    }
    return true;
}

void GstDecoderImpl::stop()
{
    if (m_pipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == ret)
        {
            detail::error("set pipeline to NULL state failed");
        }

        gst_bus_remove_watch(GST_ELEMENT_BUS(m_pipeline));

        gst_object_unref(m_appsink);
        g_object_unref(m_pipeline);
        m_pipeline = nullptr;
    }
}

gboolean GstDecoderImpl::device_monitor_bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    auto impl = static_cast<GstDecoderImpl*>(data);

    EGTLOG_TRACE("gst message: {}", GST_MESSAGE_TYPE_NAME(message));

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_DEVICE_ADDED:
    {
        GstDevice* device;
        gst_message_parse_device_added(message, &device);

        const std::string devnode = gstreamer_get_device_path(device);

        if (std::find(impl->m_devices.begin(), impl->m_devices.end(), devnode) != impl->m_devices.end())
            break;

        impl->m_devices.push_back(devnode);

        if (Application::check_instance())
        {
            asio::post(Application::instance().event().io(), [impl, devnode]()
            {
                impl->on_connect.invoke(devnode);
            });
        }

        break;
    }
    case GST_MESSAGE_DEVICE_REMOVED:
    {
        GstDevice* device;
        gst_message_parse_device_removed(message, &device);

        const std::string devnode = gstreamer_get_device_path(device);
        const auto i = std::find(impl->m_devices.begin(), impl->m_devices.end(), devnode);
        if (i == impl->m_devices.end())
            break;

        impl->m_devices.erase(i);

        /**
         * invoke disconnect only if current device is
         * disconnected.
         */
        if (devnode != impl->m_devnode)
            break;

        asio::post(Application::instance().event().io(), [impl, devnode]()
        {
            impl->on_disconnect.invoke(devnode);
        });
        break;
    }
    default:
        break;
    }

    /* we want to be notified again if there is a message on the bus, so
     * returning true (false means we want to stop watching for messages
     * on the bus and our callback should not be called again)
     */
    return true;
}

void GstDecoderImpl::get_camera_device_caps()
{
    GList* devlist = gst_device_monitor_get_devices(m_device_monitor);
    for (GList* i = g_list_first(devlist); i; i = g_list_next(i))
    {
        auto device = static_cast<GstDevice*>(i->data);
        if (device == nullptr)
            continue;

        // Probe all device properties and store them internally:
        GstStringHandle display_name{gst_device_get_display_name(device)};
        EGTLOG_DEBUG("name : {}", display_name.get());

        GstStringHandle dev_string{gst_device_get_device_class(device)};
        EGTLOG_DEBUG("class : {}", dev_string.get());

        if (gstreamer_get_device_path(device) != m_devnode)
            continue;

        GstCaps* caps = gst_device_get_caps(device);
        if (caps)
        {
            m_resolutions.clear();
            int size = gst_caps_get_size(caps);
            EGTLOG_DEBUG("caps : ");
            for (int j = 0; j < size; ++j)
            {
                GstStructure* s = gst_caps_get_structure(caps, j);
                std::string name = std::string(gst_structure_get_name(s));
                if (name == "video/x-raw")
                {
                    int width = 0;
                    int height = 0;
                    m_caps_name = name;
                    gst_structure_get_int(s, "width", &width);
                    gst_structure_get_int(s, "height", &height);
                    const gchar* str = gst_structure_get_string(s, "format");
                    m_caps_format = str ? str : "";
                    m_resolutions.emplace_back(std::make_tuple(width, height));
                    EGTLOG_DEBUG("{}, format=(string){}, width=(int){}, "
                                 "height=(int){}", m_caps_name, m_caps_format, width, height);
                }
            }

            if (!m_resolutions.empty())
            {
                // sort by camera width
                std::sort(m_resolutions.begin(), m_resolutions.end(), [](
                              std::tuple<int, int>& t1,
                              std::tuple<int, int>& t2)
                {
                    return std::get<0>(t1) < std::get<0>(t2);
                });
            }
            gst_caps_unref(caps);
        }
    }
    g_list_free(devlist);
}

std::vector<std::string> GstDecoderImpl::list_devices()
{
    return get_camera_device_list();
}

void GstDecoderImpl::device(const std::string& device)
{
    if ((m_devnode != device) || !m_pipeline)
    {
        stop();
        m_devnode = device;
        start();
    }
}

std::string GstDecoderImpl::device() const
{
    return m_devnode;
}

std::vector<std::string> GstDecoderImpl::get_camera_device_list()
{
    return m_devices;
}

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
