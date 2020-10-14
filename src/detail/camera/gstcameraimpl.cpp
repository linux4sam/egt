/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/camera/gstcameraimpl.h"
#include "detail/egtlog.h"
#include "detail/video/gstmeta.h"
#include "egt/app.h"
#ifdef HAVE_LIBPLANES
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/detail/screen/kmsscreen.h"
#endif
#include "egt/types.h"
#include "egt/video.h"
#include <exception>
#include <gst/gst.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

CameraImpl::CameraImpl(CameraWindow& interface, const Rect& rect,
                       // NOLINTNEXTLINE(modernize-pass-by-value)
                       const std::string& device)
    : m_interface(interface),
      m_devnode(device),
      m_rect(rect)
{
    static constexpr auto plugins =
    {
        "libgstcoreelements.so",
        "libgsttypefindfunctions.so",
        "libgstplayback.so",
        "libgstapp.so",
        "libgstvideo4linux2.so",
        "libgstvideoscale.so",
        "libgstvideoconvert.so",
        "libgstlibav.so",
        "libgstvideoparsersbad.so",
        "libgstaudioparsers.so",
        "libgstaudiorate.so",
        "libgstaudioconvert.so",
        "libgstaudioresample.so",
        "libgstautodetect.so",
    };
    detail::gst_init_plugins(plugins);

    m_gmain_loop = g_main_loop_new(nullptr, FALSE);
    m_gmain_thread = std::thread(g_main_loop_run, m_gmain_loop);
}

gboolean CameraImpl::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
{
    ignoreparam(bus);

    auto impl = static_cast<CameraImpl*>(data);

    EGTLOG_TRACE("gst message: {}", GST_MESSAGE_TYPE_NAME(message));

    switch (GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
    {
        GstErrorHandle error;
        GstStringHandle debug;
        gst_message_parse(gst_message_parse_error, message, error, debug);
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
        gst_message_parse(gst_message_parse_warning, message, error, debug);
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
        gst_message_parse(gst_message_parse_info, message, error, debug);
        if (error)
        {
            EGTLOG_DEBUG("gst info: {} {}",
                         error->message,
                         debug ? debug.get() : "");
        }
        break;
    }
    case GST_MESSAGE_DEVICE_ADDED:
    {
        GstDevice* device;
        gst_message_parse_device_added(message, &device);

        std::string devnode;
        GstStructure* props = gst_device_get_properties(device);
        if (props)
        {
            EGTLOG_DEBUG("device properties: {}", gst_structure_to_string(props));
            devnode = gst_structure_get_string(props, "device.path");
            gst_structure_free(props);
        }

        if (Application::check_instance())
        {
            asio::post(Application::instance().event().io(), [impl, devnode]()
            {
                impl->m_interface.on_connect.invoke(devnode);
            });
        }

        break;
    }
    case GST_MESSAGE_DEVICE_REMOVED:
    {
        GstDevice* device;
        gst_message_parse_device_removed(message, &device);
        std::string devnode;
        GstStructure* props = gst_device_get_properties(device);
        if (props)
        {
            EGTLOG_DEBUG("device properties: {}", gst_structure_to_string(props));
            devnode = gst_structure_get_string(props, "device.path");
            gst_structure_free(props);
        }

        asio::post(Application::instance().event().io(), [impl, devnode]()
        {
            /**
             * invoke disconnect only if current device is
             * disconnected.
             */
            if (devnode == impl->m_devnode)
                impl->m_interface.on_disconnect.invoke(devnode);
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

        EGTLOG_TRACE("videowidth = {}  videoheight = {}", width, height);

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
    auto impl = static_cast<CameraImpl*>(data);

    GstSample* sample;
    g_signal_emit_by_name(elt, "pull-sample", &sample);
    if (sample)
    {
#ifdef HAVE_LIBPLANES
        // TODO: this is not thread safe accessing impl here
        if (impl->m_interface.plane_window())
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
            if (Application::check_instance())
            {
                asio::post(Application::instance().event().io(), [impl, sample]()
                {
                    if (impl->m_camerasample)
                        gst_sample_unref(impl->m_camerasample);

                    impl->m_camerasample = sample;
                    impl->m_interface.damage();
                });
            }
        }
        return GST_FLOW_OK;
    }
    return GST_FLOW_ERROR;
}

void CameraImpl::get_camera_device_caps()
{
    std::tuple<std::string, std::string, std::string,
        std::vector<std::tuple<int, int>>> caps = detail::get_camera_device_caps(m_devnode, &bus_callback, this);

    m_devnode = std::get<0>(caps);
    m_caps_name = std::get<1>(caps);
    m_caps_format = std::get<2>(caps);
    m_resolutions = std::get<3>(caps);
}

bool CameraImpl::start()
{
    std::string pipe;

    get_camera_device_caps();

    auto box = m_interface.content_area();
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
        m_rect.size(Size(32, 32));
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

    const auto format = detail::gstreamer_format(m_interface.format());
    EGTLOG_DEBUG("format: {}  ", format);

    static constexpr auto appsink_pipe =
        "v4l2src device={} ! videoconvert ! video/x-raw,width={},height={},format={} ! {} " \
        "appsink name=appsink async=false enable-last-sample=false sync=true";

    pipe = fmt::format(appsink_pipe, m_devnode, w, h, format, vscale);

    EGTLOG_DEBUG(pipe);

    /* Make sure we don't leave orphan references */
    stop();

    GError* error = nullptr;
    m_pipeline = gst_parse_launch(pipe.c_str(), &error);
    if (!m_pipeline)
    {
        m_interface.on_error.invoke(fmt::format("failed to create pipeline: {}", error->message));
        return false;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "appsink");
    if (!m_appsink)
    {
        m_interface.on_error.invoke("failed to get app sink element");
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

    int ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        m_interface.on_error.invoke("failed to set pipeline to play state");
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
            detail::error("set pipeline to NULL state failed");
        }
        g_object_unref(m_pipeline);
        m_pipeline = nullptr;
    }
}

CameraImpl::~CameraImpl() noexcept
{
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

std::tuple<std::string, std::string, std::string, std::vector<std::tuple<int, int>>>
get_camera_device_caps(const std::string& dev_name, BusCallback bus_callback, void* instance)
{
    std::string devnode;

    GstDeviceMonitor* monitor = gst_device_monitor_new();

    GstBus* bus = gst_device_monitor_get_bus(monitor);
    gst_bus_add_watch(bus, bus_callback, instance);
    gst_object_unref(bus);

    GstCaps* caps = gst_caps_new_empty_simple("video/x-raw");
    gst_device_monitor_add_filter(monitor, "Video/Source", caps);
    gst_caps_unref(caps);

    std::string caps_name;
    std::string caps_format;
    std::vector<std::tuple<int, int>> resolutions;
    if (gst_device_monitor_start(monitor))
    {
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
                        caps_format = std::string(gst_structure_get_string(s, "format"));
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

            GstStructure* props = gst_device_get_properties(device);
            if (props)
            {
                EGTLOG_DEBUG("device properties: {}", gst_structure_to_string(props));

                devnode = std::string(gst_structure_get_string(props, "device.path"));
                gst_structure_free(props);

                if (devnode == dev_name)
                    break;
            }
        }
        g_list_free(devlist);
    }

    EGTLOG_DEBUG("camera device node : {}", devnode);

    return std::make_tuple(devnode, caps_name, caps_format, resolutions);
}

}
}
}
