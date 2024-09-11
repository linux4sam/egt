/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "detail/egtlog.h"
#include "detail/multimedia/gstappsink.h"
#include "detail/multimedia/gstdecoderimpl.h"
#include "detail/multimedia/gstmeta.h"
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/app.h"
#include "egt/uri.h"
#include "egt/detail/math.h"
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

GstDecoderImpl::GstDecoderImpl(Window& iface, const Size& size)
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
        start();

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
    /*
     * This case is for custom pipeline. There is no source, let's consider
     * there is audio. For instance, it enables the display of the volume for
     * the video player example.
     */
    if (!m_src)
        return true;
    else
        return m_src->has_audio();
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
        EGTLOG_DEBUG("loopback: {}", (impl->loopback() ? "TRUE" : "FALSE"));
        if (impl->loopback())
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

void GstDecoderImpl::draw(Painter& painter, const Rect& rect)
{
    m_sink->draw(painter, rect);
}

std::string GstDecoderImpl::create_pipeline_desc()
{
    m_sink = std::make_unique<GstAppSink>(*this, m_size, m_interface);

    if (!m_custom_pipeline_desc.empty())
        return m_custom_pipeline_desc;

    const auto audio = (m_src->has_audio())
                       ? "source. ! queue ! audioconvert ! volume name=volume ! autoaudiosink sync=false"
                       : "";

    return fmt::format("{} ! {} {}", m_src->description(), m_sink->description(), audio);
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

    if (m_src)
    { 
        if (m_src->has_audio())
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
    }

    if (!m_sink->post_initialize())
    {
        destroyPipeline();
        return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    m_bus_watchid = gst_bus_add_watch(m_bus, &bus_callback, this);

    m_eventsource_id = g_timeout_add(5000, static_cast<GSourceFunc>(&post_position), this);

    return true;
}

bool GstDecoderImpl::media(const std::string& uri)
{
    if (detail::change_if_diff(m_uri, uri))
    {
        m_src = GstSrc::create(*this, m_uri, m_video_enabled, m_audio_enabled && m_audiodevice);
        if (!m_src)
            return false;
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
        if (m_pipeline)
            m_sink->size(size);

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
    /* Make sure we don't leave orphan references */
    stop();

    const auto buffer = create_pipeline_desc();
    EGTLOG_DEBUG("pipeline description: {}", buffer);

    if (!create_pipeline(buffer))
        return false;

    /*
     * GStreamer documentation states about GstParse that these functions take
     * several measures to create somewhat dynamic pipelines. Due to that such
     * pipelines are not always reusable (set the state to NULL and back to
     * PLAYING).
     */
    auto ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        on_error.invoke("failed to set pipeline to null state");
        destroyPipeline();
        return false;
    }

    return true;
}

void GstDecoderImpl::stop()
{
    destroyPipeline();
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
    media(device);
}

std::string GstDecoderImpl::device() const
{
    return m_devnode;
}

std::vector<std::string> GstDecoderImpl::get_camera_device_list()
{
    return m_devices;
}

void GstDecoderImpl::loopback(bool enable)
{
    m_loopback = enable;
}

EGT_NODISCARD bool GstDecoderImpl::loopback() const
{
    return m_loopback;
}

void GstDecoderImpl::enable_video(bool enable)
{
    m_video_enabled = enable;
}

void GstDecoderImpl::enable_audio(bool enable)
{
    m_audio_enabled = enable;
}

std::tuple<std::string, std::string, std::string, std::vector<std::tuple<int, int>>>
get_camera_device_caps(const std::string& dev_name)
{
    GstDeviceMonitor* monitor = gst_device_monitor_new();

    GstCaps* caps = gst_caps_new_empty_simple("video/x-raw");
    gst_device_monitor_add_filter(monitor, "Video/Source", caps);
    gst_caps_unref(caps);

    std::string caps_name;
    std::string caps_format;
    std::vector<std::tuple<int, int>> resolutions;

    GList* devlist = gst_device_monitor_get_devices(monitor);
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

        if (gstreamer_get_device_path(device) != dev_name)
            continue;

        caps = gst_device_get_caps(device);
        if (caps)
        {
            resolutions.clear();
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
                    caps_name = name;
                    gst_structure_get_int(s, "width", &width);
                    gst_structure_get_int(s, "height", &height);
                    const gchar* str = gst_structure_get_string(s, "format");
                    caps_format = str ? str : "";
                    resolutions.emplace_back(std::make_tuple(width, height));
                    EGTLOG_DEBUG("{}, format=(string){}, width=(int){}, "
                                 "height=(int){}", caps_name, caps_format, width, height);
                }
            }

            if (!resolutions.empty())
            {
                // sort by camera width
                std::sort(resolutions.begin(), resolutions.end(), [](
                              std::tuple<int, int>& t1,
                              std::tuple<int, int>& t2)
                {
                    return std::get<0>(t1) < std::get<0>(t2);
                });
            }
            gst_caps_unref(caps);
        }

        break;
    }
    g_list_free(devlist);

    return std::make_tuple(dev_name, caps_name, caps_format, resolutions);
}

} // end of namespace detail

} // end of namespace v1

} // end of namespace egt
