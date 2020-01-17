/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/camera/gstcameraimpl.h"
#include "detail/video/gstmeta.h"
#include "egt/app.h"
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/detail/screen/kmsscreen.h"
#include "egt/types.h"
#include "egt/video.h"
#include <exception>
#include <gst/gst.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

CameraImpl::CameraImpl(CameraWindow& interface, const Rect& rect,
                       const std::string& device, bool useKmssink)
    : m_interface(interface),
      m_devnode(device),
      m_rect(rect),
      m_usekmssink(useKmssink)
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
    /**
     * check for cache file by finding a playback plugin.
     * if gst_registry_find_plugin returns NULL, then no
     * cache file present and assume GSTREAMER1_PLUGIN_REGISTRY
     * is disabled in gstreamer package.
     */
    if (!gst_registry_find_plugin(gst_registry_get(), "playback"))
    {
        SPDLOG_DEBUG(" Manually loading gstreamer plugins");
        auto plugins =
        {
            "/usr/lib/gstreamer-1.0/libgstcoreelements.so",
            "/usr/lib/gstreamer-1.0/libgsttypefindfunctions.so",
            "/usr/lib/gstreamer-1.0/libgstplayback.so",
            "/usr/lib/gstreamer-1.0/libgstapp.so",
            "/usr/lib/gstreamer-1.0/libgstvideo4linux2.so",
            "/usr/lib/gstreamer-1.0/libgstvideoscale.so",
            "/usr/lib/gstreamer-1.0/libgstvideoconvert.so",
            "/usr/lib/gstreamer-1.0/libgstlibav.so",
            "/usr/lib/gstreamer-1.0/libgstvideoparsersbad.so",
        };

        for (auto& plugin : plugins)
        {
            GError* error = nullptr;
            gst_plugin_load_file(plugin, &error);
            if (error)
            {
                if (error->message)
                    spdlog::error("load plugin error: {}", error->message);
                g_error_free(error);
            }
        }
    }

    m_gmainLoop = g_main_loop_new(nullptr, FALSE);
    m_gmainThread = std::thread(g_main_loop_run, m_gmainLoop);

}

gboolean CameraImpl::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    auto impl = reinterpret_cast<CameraImpl*>(data);

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
    case GST_MESSAGE_DEVICE_ADDED:
    {
        GstDevice* device;
        gst_message_parse_device_added(message, &device);
        gchar* name = gst_device_get_display_name(device);
        SPDLOG_DEBUG("Device added: {} \n", name);
        g_free(name);
        gst_object_unref(device);

        asio::post(Application::instance().event().io(), [impl]()
        {
            if (impl->start())
            {
                impl->m_err_message = "";
                impl->m_interface.on_error.invoke();
            }
        });

        break;
    }
    case GST_MESSAGE_DEVICE_REMOVED:
    {
        GstDevice* device;
        gst_message_parse_device_removed(message, &device);
        gchar* name = gst_device_get_display_name(device);
        SPDLOG_DEBUG("Device removed: {} \n", name);
        g_free(name);
        gst_object_unref(device);

        asio::post(Application::instance().event().io(), [impl, name]()
        {
            impl->m_err_message = std::string(name) + " Device removed";
            impl->m_interface.on_error.invoke();
        });
        impl->stop();
        break;
    }
    default:
    {
        SPDLOG_DEBUG("default Message {}", std::to_string(GST_MESSAGE_TYPE(message)));
        break;
    }
    }

    /* we want to be notified again if there is a message on the bus, so
     * returning true (false means we want to stop watching for messages
     * on the bus and our callback should not be called again)
     */
    return true;
}

/*
 * Its a Basic window: copying buffer to cairo surface.
 */
void CameraImpl::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    if (m_camerasample)
    {
        GstCaps* caps = gst_sample_get_caps(m_camerasample);
        GstStructure* capsStruct = gst_caps_get_structure(caps, 0);
        int width = 0;
        int height = 0;
        gst_structure_get_int(capsStruct, "width", &width);
        gst_structure_get_int(capsStruct, "height", &height);

        SPDLOG_TRACE("videowidth = {}  videoheight = {}", width, height);

        gst_sample_ref(m_camerasample);
        GstBuffer* buffer = gst_sample_get_buffer(m_camerasample);

        GstMapInfo map;
        if (gst_buffer_map(buffer, &map, GST_MAP_READ))
        {
            const auto box = m_interface.content_area();
            const auto surface = unique_cairo_surface_t(
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
            gst_buffer_unmap(buffer, &map);
        }
        gst_sample_unref(m_camerasample);
    }
}

GstFlowReturn CameraImpl::on_new_buffer(GstElement* elt, gpointer data)
{
    auto impl = reinterpret_cast<CameraImpl*>(data);

    GstSample* sample;
    g_signal_emit_by_name(elt, "pull-sample", &sample);
    if (sample)
    {
#ifdef HAVE_LIBPLANES
        // TODO: this is not thread safe accessing impl here
        if (impl->m_interface.flags().is_set(Widget::Flag::plane_window))
        {
            GstBuffer* buffer = gst_sample_get_buffer(sample);
            if (buffer)
            {
                GstMapInfo map;
                if (gst_buffer_map(buffer, &map, GST_MAP_READ))
                {
                    auto screen =
                        reinterpret_cast<detail::KMSOverlay*>(impl->m_interface.screen());
                    assert(screen);
                    if (screen)
                    {
                        memcpy(screen->raw(), map.data, map.size);
                        screen->schedule_flip();
                        gst_buffer_unmap(buffer, &map);
                    }
                }
            }
            gst_sample_unref(sample);
        }
        else
#endif
        {
            asio::post(Application::instance().event().io(), [impl, sample]()
            {
                if (impl->m_camerasample)
                    gst_sample_unref(impl->m_camerasample);

                impl->m_camerasample = sample;
                impl->m_interface.damage();
            });
        }
        return GST_FLOW_OK;
    }
    return GST_FLOW_ERROR;
}

void CameraImpl::get_camera_device_caps()
{

    GstDeviceMonitor* monitor = gst_device_monitor_new();

    GstBus* bus = gst_device_monitor_get_bus(monitor);
    gst_bus_add_watch(bus, &bus_callback, this);
    gst_object_unref(bus);

    GstCaps* caps = gst_caps_new_empty_simple("video/x-raw");
    gst_device_monitor_add_filter(monitor, "Video/Source", caps);
    gst_caps_unref(caps);

    if (gst_device_monitor_start(monitor))
    {
        GList* devlist = gst_device_monitor_get_devices(monitor);
        for (GList* i = g_list_first(devlist); i; i = g_list_next(i))
        {
            auto device = static_cast<GstDevice*>(i->data);
            if (device == nullptr)
                continue;

            // Probe all device properties and store them internally:
            auto display_name = gst_device_get_display_name(device);
            SPDLOG_DEBUG("In {} : display_name = {} \n", __func__, display_name);
            g_free(display_name);

            auto devString = gst_device_get_device_class(device);
            SPDLOG_DEBUG("In {} : devString = {} \n", __func__, devString);
            g_free(devString);

            GstStructure* props = gst_device_get_properties(device);
            if (props)
            {
                SPDLOG_DEBUG("In {} : Device Properties: \n {} \n", __func__, gst_structure_to_string(props));

                m_devnode = gst_structure_get_string(props, "device.path");
                SPDLOG_DEBUG("In {} : Using default Camera device = {} \n", __func__, m_devnode);
                gst_structure_free(props);
            }
        }
        g_list_free(devlist);
    }
}

bool CameraImpl::start()
{
    std::string pipe;

    get_camera_device_caps();

#ifdef HAVE_LIBPLANES
    if (m_interface.flags().is_set(Widget::Flag::plane_window) && m_usekmssink)
    {
        auto s = reinterpret_cast<detail::KMSOverlay*>(m_interface.screen());
        assert(s);
        if (s)
        {
            static const auto kmssink_pipe =
                "v4l2src device={} ! videoconvert ! video/x-raw,width={},height={},format={},framerate=15/1 ! " \
                "g1kmssink gem-name={}";


            const auto gem_name = s->gem();
            const auto format = detail::gstreamer_format(m_interface.format());
            const auto box = m_interface.content_area();
            pipe = fmt::format(kmssink_pipe, m_devnode, box.width(), box.height(), format, gem_name);
        }
    }
    else
#endif
    {
        static const auto appsink_pipe =
            "v4l2src device={} ! videoconvert ! video/x-raw,width={},height={},format={},framerate=15/1 ! " \
            "appsink name=appsink async=false enable-last-sample=false sync=true";

        const auto box = m_interface.content_area();
        const auto format = detail::gstreamer_format(m_interface.format());
        pipe = fmt::format(appsink_pipe, m_devnode, box.width(), box.height(), format);
    }

    SPDLOG_DEBUG(pipe);

    /* Make sure we don't leave orphan references */
    stop();

    GError* error = nullptr;
    m_pipeline = gst_parse_launch(pipe.c_str(), &error);
    if (!m_pipeline)
    {
        m_err_message = error->message;
        spdlog::error("failed to create pipeline: {}", m_err_message);
        m_interface.on_error.invoke();
        return false;
    }

    if (!m_usekmssink)
    {
        m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "appsink");
        if (!m_appsink)
        {
            m_err_message = "failed to get app sink element";
            spdlog::error(m_err_message);
            m_interface.on_error.invoke();
            return false;
        }

        g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, nullptr);
        g_signal_connect(m_appsink, "new-sample", G_CALLBACK(on_new_buffer), this);
    }

    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    gst_bus_add_watch(bus, &bus_callback, this);
    gst_object_unref(bus);

    int ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        m_err_message = "failed to set pipeline to play state";
        spdlog::error(m_err_message);
        m_interface.on_error.invoke();
        stop();
        return false;
    }
    return true;
}

void CameraImpl::scale(float scalex, float scaley)
{
    m_interface.resize(Size(m_rect.width() * scalex, m_rect.height() * scaley));
}

void CameraImpl::stop()
{
    if (m_pipeline)
    {
        GstStateChangeReturn ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == ret)
        {
            spdlog::error("set pipeline to NULL state failed");
        }
        g_object_unref(m_pipeline);
        m_pipeline = nullptr;
    }
}

std::string CameraImpl::error_message() const
{
    return m_err_message;
}

CameraImpl::~CameraImpl()
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

}
}
}
