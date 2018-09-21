/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "video.h"
#include "kmsscreen.h"
#include <string.h>
#include <thread>
#include <chrono>

#ifdef HAVE_GSTREAMER

#include <gst/app/gstappsink.h>

using namespace std;

#define SRC_NAME "srcVideo"
#define PERF_NAME "perf"
#define PROGRESS_NAME "progress"
#define VOLUME_NAME "volume"

static void init_thread()
{
    static bool init = false;
    if (!init)
    {
        static std::thread t([]()
        {
            GMainLoop* mainloop = g_main_loop_new(NULL, FALSE);
            g_main_loop_run(mainloop);
        });
        init = true;
    }
}

namespace mui
{

    gboolean VideoWindow::bus_callback(GstBus* bus, GstMessage* message, gpointer data)
    {
        VideoWindow* _this = (VideoWindow*)data;

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
        {
            if (!strncmp(GST_MESSAGE_SRC_NAME(message), PERF_NAME, 4))
            {
                GError* error = NULL;
                gchar* debug = NULL;
                gchar* fps = NULL;

                gst_message_parse_info(message, &error, &debug);
                if (debug)
                {
                    fps = g_strrstr(debug, "fps: ") + 5;
                    _this->m_fps = atoi(fps);
                }

                g_error_free(error);
                g_free(debug);
            }
            break;
        }
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

            _this->m_fps = 0;

            // TODO: remove me, loop
            gst_element_seek(_this->m_video_pipeline, 1.0, GST_FORMAT_TIME,
                             GST_SEEK_FLAG_FLUSH,
                             GST_SEEK_TYPE_SET, 0,
                             GST_SEEK_TYPE_NONE, -1);

            _this->set_state(GST_STATE_PLAYING);

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

    VideoWindow::VideoWindow(const Size& size, uint32_t format, bool heo)
        : PlaneWindow(size, FLAG_WINDOW_DEFAULT | FLAG_NO_BACKGROUND, format, heo),
          m_video_pipeline(NULL),
          m_src(NULL),
          m_volume(NULL),
          m_position(0),
          m_duration(0),
          m_fps(0)
    {
        gst_init(NULL, NULL);
        init_thread();
    }

    void VideoWindow::scale(float value)
    {
        KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
        screen->scale(value);
    }

    float VideoWindow::scale() const
    {
        KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
        return screen->scale();
    }

    void VideoWindow::draw()
    {
        // don't do any drawing to a window because we don't support video formats, only positioning
        KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(m_screen);
        s->apply();
        m_dirty = false;
        m_damage.clear();
    }

    void VideoWindow::destroyPipeline()
    {
        null();

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

        if (m_video_pipeline)
        {
            g_object_unref(m_video_pipeline);
            m_video_pipeline = NULL;
        }
    }

    VideoWindow::~VideoWindow()
    {
        destroyPipeline();
    }

    bool VideoWindow::play(bool mute, int volume)
    {
        set_state(GST_STATE_PLAYING);
        return false;
    }

    bool VideoWindow::unpause()
    {
        set_state(GST_STATE_PLAYING);
        return false;
    }

    bool VideoWindow::pause()
    {
        m_fps = 0;
        set_state(GST_STATE_PAUSED);
        return false;
    }

    bool VideoWindow::null()
    {
        m_fps = 0;
        return set_state(GST_STATE_NULL);
    }

    bool VideoWindow::set_media(const string& uri)
    {
        m_filename = uri;

        destroyPipeline();
        createPipeline();
        //g_object_set(m_src, "uri", (string("file://") + uri).c_str(), NULL);
        //g_object_set(m_src, "location", uri.c_str(), NULL);
        g_object_set(m_src, "device", uri.c_str(), NULL);

        return true;
    }

    bool VideoWindow::set_volume(int volume)
    {
        if (!m_volume)
            return false;

        if (volume < 1)
            volume = 1;
        if (volume > 100)
            volume = 100;

        g_object_set(m_volume, "volume", volume / 100.0, NULL);
        invoke_handlers();

        return true;
    }

    int VideoWindow::get_volume() const
    {
        if (!m_volume)
            return 0;

        gdouble volume = 0;
        g_object_get(m_volume, "volume", &volume, NULL);
        return volume * 100.0;
    }

    bool VideoWindow::set_mute(bool mute)
    {
        if (!m_volume)
            return false;

        g_object_set(m_volume, "mute", mute, NULL);
        invoke_handlers();
        return true;
    }

    bool VideoWindow::set_state(GstState state)
    {
        GstStateChangeReturn ret;

        if (m_video_pipeline)
        {
            ret = gst_element_set_state(m_video_pipeline, state);
            if (GST_STATE_CHANGE_FAILURE == ret)
            {
                ERR("unable to set video pipeline to " << state);
                return false;
            }

            invoke_handlers();
        }
        else
        {
            return false;
        }

        return true;
    }

    bool VideoWindow::playing() const
    {
        GstState state = GST_STATE_VOID_PENDING;

        if (m_video_pipeline)
        {
            (void)gst_element_get_state(m_video_pipeline, &state,
                                        NULL, GST_CLOCK_TIME_NONE);
            return state == GST_STATE_PLAYING;
        }

        return false;
    }


#define HARDWAREPIPE "uridecodebin expose-all-streams=false name=" SRC_NAME " caps=video/x-h264;audio/x-raw " \
    SRC_NAME ". ! queue ! h264parse ! g1h264dec ! video/x-raw,width=%d,height=%d,format=YUY2 ! " \
    "progressreport silent=true do-query=true update-freq=1 format=time	name=" PROGRESS_NAME " ! " \
    "perf name=" PERF_NAME " ! " \
    "g1kmssink gem-name=%d " \
    SRC_NAME ". ! queue ! audioconvert ! volume name=" VOLUME_NAME " ! " \
    "alsasink async=false enable-last-sample=false sync=true"

    HardwareVideo::HardwareVideo(const Size& size, uint32_t format)
        : VideoWindow(size, format /*DRM_FORMAT_XRGB8888*/ /*DRM_FORMAT_YUYV*/, true)
    {
    }

    bool HardwareVideo::createPipeline()
    {
        GError* error = NULL;
        GstBus* bus;
        //guint bus_watch_id;

        KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
        int _gem = screen->gem();

        /* Make sure we don't leave orphan references */
        destroyPipeline();

        char buffer[2048];
        sprintf(buffer, HARDWAREPIPE, w(), h(), _gem);

        DBG("gem = " << _gem << " w()=" << w() << " h()=" << h());

        string pipe(buffer);
        DBG(pipe);

        m_video_pipeline = gst_parse_launch(pipe.c_str(), &error);
        if (!m_video_pipeline)
        {
            m_video_pipeline = NULL;
            ERR("failed to create video pipeline");
            return false;
        }

        m_src = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SRC_NAME);
        if (!m_src)
        {
            m_src = NULL;
            ERR("failed to get video src element");
            return false;
        }

        m_volume = gst_bin_get_by_name(GST_BIN(m_video_pipeline), VOLUME_NAME);
        if (!m_volume)
        {
            m_volume = NULL;
            ERR("failed to get volume element");
            return false;
        }

        bus = gst_pipeline_get_bus(GST_PIPELINE(m_video_pipeline));
        /*bus_watch_id =*/ gst_bus_add_watch(bus, &bus_callback, this);
        gst_object_unref(bus);

        return true;
    }

    HardwareVideo::~HardwareVideo()
    {

    }

    bool HardwareVideo::set_media(const string& uri)
    {
        m_filename = uri;

        destroyPipeline();
        createPipeline();
        g_object_set(m_src, "uri", (string("file://") + uri).c_str(), NULL);

        return true;
    }

#if 1

#define V4L2HARDWAREPIPE "v4l2src name=" SRC_NAME " do-timestamp=true ! image/jpeg,width=%d,framerate=30/1 ! " \
    "jpegparse ! jpegdec ! video/x-raw,format=I420,framerate=30/1 ! " \
    "perf name=" PERF_NAME " ! " \
    "g1kmssink gem-name=%d"
#define FORMAT DRM_FORMAT_YUV420

#else

#define V4L2HARDWAREPIPE "v4l2src name=" SRC_NAME " do-timestamp=true ! image/jpeg,width=%d,framerate=30/1 ! " \
    "jpegparse ! g1jpegdec ! video/x-raw,width=960,height=720,format=YUY2 ! " \
    "perf name=" PERF_NAME " ! " \
    "g1kmssink gem-name=%d sync=true"
#define FORMAT DRM_FORMAT_YUYV
    //#define FORMAT DRM_FORMAT_RGBA8888

#endif

    V4L2HardwareVideo::V4L2HardwareVideo(const Size& size)
        : HardwareVideo(size, FORMAT)
    {
    }

    bool V4L2HardwareVideo::createPipeline()
    {
        GError* error = NULL;
        GstBus* bus;
        //guint bus_watch_id;

        KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
        int _gem = screen->gem();

        /* Make sure we don't leave orphan references */
        destroyPipeline();

        char buffer[2048];
        sprintf(buffer, V4L2HARDWAREPIPE, w(), _gem);

        string pipe(buffer);
        DBG(pipe);

        m_video_pipeline = gst_parse_launch(pipe.c_str(), &error);
        if (!m_video_pipeline)
        {
            m_video_pipeline = NULL;
            ERR("failed to create video pipeline");
            return false;
        }

        m_src = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SRC_NAME);
        if (!m_src)
        {
            m_src = NULL;
            ERR("failed to get video src element");
            return false;
        }

        m_volume = gst_bin_get_by_name(GST_BIN(m_video_pipeline), VOLUME_NAME);
        if (!m_volume)
        {
            ERR("failed to get volume element");
        }

        bus = gst_pipeline_get_bus(GST_PIPELINE(m_video_pipeline));
        /*bus_watch_id =*/ gst_bus_add_watch(bus, &bus_callback, this);
        gst_object_unref(bus);

        GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_video_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

        return true;
    }

    V4L2HardwareVideo::~V4L2HardwareVideo()
    {

    }

    bool V4L2HardwareVideo::set_media(const string& uri)
    {
        m_filename = uri;

        destroyPipeline();
        createPipeline();
        g_object_set(m_src, "device", uri.c_str(), NULL);

        return true;
    }


#define SINK_NAME "testsink"

#define SOFTWAREPIPE "uridecodebin expose-all-streams=false name=" SRC_NAME " caps=video/x-raw;audio/x-raw use-buffering=true buffer-size=1048576 " \
    SRC_NAME ". ! queue ! autovideoconvert ! video/x-raw,width=%d,height=%d ! " \
    "progressreport silent=true do-query=true update-freq=1 format=time name=" PROGRESS_NAME " ! " \
    "perf name=" PERF_NAME " ! " \
    "appsink drop=true enable-last-sample=false caps=video/x-raw name=" SINK_NAME " " \
    SRC_NAME ". ! queue ! audioconvert ! volume name=" VOLUME_NAME " ! " \
    "alsasink async=false enable-last-sample=false sync=true"

    GstFlowReturn SoftwareVideo::on_new_buffer_from_source(GstElement* elt, gpointer data)
    {
        SoftwareVideo* _this = (SoftwareVideo*)data;

#if 0
        GstCaps* caps = gst_pad_get_current_caps(GST_BASE_SINK_PAD(elt));
        GstStructure* props = gst_caps_get_structure(caps, 0);
        for (int x = 0; x < gst_structure_n_fields(props); x++)
        {
            const gchar* fieldname = gst_structure_nth_field_name(props, x);
            if (g_strrstr(fieldname, "width"))
            {
                gint width = 0;
                if (gst_structure_get_int(props, fieldname, &width))
                    cout << "width:" << width << endl;
            }
            else if (g_strrstr(fieldname, "height"))
            {
                gint height = 0;
                if (gst_structure_get_int(props, fieldname, &height))
                    cout << "height:" << height << endl;
            }
        }
#endif

        GstSample* sample;

        g_signal_emit_by_name(elt, "pull-sample", &sample);

        if (sample)
        {
            GstBuffer* buffer = gst_sample_get_buffer(sample);
            GstMapInfo map;

            if (gst_buffer_map(buffer, &map, GST_MAP_READ))
            {
                KMSOverlayScreen* screen =
                    reinterpret_cast<KMSOverlayScreen*>(_this->m_screen);
                //cout << "frame size: " << map.size << endl;
                memcpy(screen->raw(), map.data, map.size);
                screen->schedule_flip();
                gst_buffer_unmap(buffer, &map);
            }

            gst_sample_unref(sample);
        }

        return GST_FLOW_OK;
    }

    SoftwareVideo::SoftwareVideo(const Size& size, uint32_t format)
        : VideoWindow(size, format),
          m_appsink(NULL)
    {
    }

    bool SoftwareVideo::createPipeline()
    {
        GError* error = NULL;
        GstBus* bus;
        //guint bus_watch_id;

        /* Make sure we don't leave orphan references */
        destroyPipeline();

        char buffer[2048];
        sprintf(buffer, SOFTWAREPIPE, w(), h());

        string pipe(buffer);
        DBG(pipe);

        m_video_pipeline = gst_parse_launch(pipe.c_str(), &error);
        if (!m_video_pipeline)
        {
            m_video_pipeline = NULL;
            ERR("failed to create video pipeline");
            return false;
        }

        m_src = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SRC_NAME);
        if (!m_src)
        {
            m_src = NULL;
            ERR("failed to get video src element");
            return false;
        }

        m_volume = gst_bin_get_by_name(GST_BIN(m_video_pipeline), VOLUME_NAME);
        if (!m_volume)
        {
            ERR("failed to get volume element");
        }

        m_appsink = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SINK_NAME);
        if (!m_appsink)
        {
            ERR("failed to get test sink element");
            return false;
        }

        g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, NULL);
        g_signal_connect(m_appsink, "new-sample",
                         G_CALLBACK(on_new_buffer_from_source), this);


        bus = gst_pipeline_get_bus(GST_PIPELINE(m_video_pipeline));
        /*bus_watch_id =*/ gst_bus_add_watch(bus, &bus_callback, this);
        gst_object_unref(bus);

        return true;
    }

    void SoftwareVideo::destroyPipeline()
    {
        VideoWindow::destroyPipeline();

        if (m_appsink)
        {
            g_object_unref(m_appsink);
            m_appsink = NULL;
        }
    }

    SoftwareVideo::~SoftwareVideo()
    {

    }

    bool SoftwareVideo::set_media(const string& uri)
    {
        m_filename = uri;

        destroyPipeline();
        createPipeline();
        g_object_set(m_src, "uri", (string("file://") + uri).c_str(), NULL);

        return true;
    }


#define V4L2SOFTWAREPIPE "v4l2src name=" SRC_NAME " ! video/x-raw,width=%d,framerate=15/1 ! " \
    "perf name=" PERF_NAME " ! " \
    "appsink drop=true enable-last-sample=false caps=video/x-raw name=" SINK_NAME

    V4L2SoftwareVideo::V4L2SoftwareVideo(const Size& size)
        : SoftwareVideo(size, DRM_FORMAT_YUYV)
    {
    }

    bool V4L2SoftwareVideo::createPipeline()
    {
        GError* error = NULL;
        GstBus* bus;
        //guint bus_watch_id;

        /* Make sure we don't leave orphan references */
        destroyPipeline();

        char buffer[2048];
        sprintf(buffer, V4L2SOFTWAREPIPE, w());

        string pipe(buffer);
        DBG(pipe);

        m_video_pipeline = gst_parse_launch(pipe.c_str(), &error);
        if (!m_video_pipeline)
        {
            m_video_pipeline = NULL;
            ERR("failed to create video pipeline");
            return false;
        }

        m_src = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SRC_NAME);
        if (!m_src)
        {
            m_src = NULL;
            ERR("failed to get video src element");
            return false;
        }

        m_volume = gst_bin_get_by_name(GST_BIN(m_video_pipeline), VOLUME_NAME);
        if (!m_volume)
        {
            ERR("failed to get volume element");
        }

        m_appsink = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SINK_NAME);
        if (!m_appsink)
        {
            ERR("failed to get test sink element");
            return false;
        }

        g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, NULL);
        g_signal_connect(m_appsink, "new-sample",
                         G_CALLBACK(on_new_buffer_from_source), this);


        bus = gst_pipeline_get_bus(GST_PIPELINE(m_video_pipeline));
        /*bus_watch_id =*/ gst_bus_add_watch(bus, &bus_callback, this);
        gst_object_unref(bus);

        GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(m_video_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

        return true;
    }

    V4L2SoftwareVideo::~V4L2SoftwareVideo()
    {

    }

    bool V4L2SoftwareVideo::set_media(const string& uri)
    {
        m_filename = uri;

        destroyPipeline();
        createPipeline();
        g_object_set(m_src, "device", uri.c_str(), NULL);

        return true;
    }


#define RAWSOFTWAREPIPE "filesrc name=" SRC_NAME " ! videoparse width=%d height=%d framerate=24/1 format=nv21 " \
    " ! autovideoconvert ! "						\
    "progressreport silent=true do-query=true update-freq=1 format=time name=" PROGRESS_NAME " ! " \
    "perf name=" PERF_NAME " ! " \
    "appsink drop=true enable-last-sample=false caps=video/x-raw name=" SINK_NAME

    RawSoftwareVideo::RawSoftwareVideo(const Size& size)
        : SoftwareVideo(size)
    {}

    bool RawSoftwareVideo::createPipeline()
    {
        GError* error = NULL;
        GstBus* bus;
        //guint bus_watch_id;

        /* Make sure we don't leave orphan references */
        destroyPipeline();

        char buffer[2048];
        sprintf(buffer, RAWSOFTWAREPIPE, w(), h());

        string pipe(buffer);
        DBG(pipe);

        m_video_pipeline = gst_parse_launch(pipe.c_str(), &error);
        if (!m_video_pipeline)
        {
            m_video_pipeline = NULL;
            ERR("failed to create video pipeline");
            return false;
        }

        m_src = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SRC_NAME);
        if (!m_src)
        {
            m_src = NULL;
            ERR("failed to get video src element");
            return false;
        }

        m_volume = gst_bin_get_by_name(GST_BIN(m_video_pipeline), VOLUME_NAME);
        if (!m_volume)
        {
            ERR("failed to get volume element");
        }

        m_appsink = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SINK_NAME);
        if (!m_appsink)
        {
            ERR("failed to get test sink element");
            return false;
        }

        g_object_set(G_OBJECT(m_appsink), "emit-signals", TRUE, "sync", TRUE, NULL);
        g_signal_connect(m_appsink, "new-sample",
                         G_CALLBACK(on_new_buffer_from_source), this);


        bus = gst_pipeline_get_bus(GST_PIPELINE(m_video_pipeline));
        /*bus_watch_id =*/ gst_bus_add_watch(bus, &bus_callback, this);
        gst_object_unref(bus);

        return true;
    }

    RawSoftwareVideo::~RawSoftwareVideo()
    {

    }

    bool RawSoftwareVideo::set_media(const string& uri)
    {
        m_filename = uri;

        destroyPipeline();
        createPipeline();
        g_object_set(m_src, "location", uri.c_str(), NULL);

        return true;
    }
}

#endif
