/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "video.h"
#include "kmsscreen.h"

#ifdef HAVE_GSTREAMER

using namespace std;

#define SRC_NAME "srcVideo"
#define PERF_NAME "perf"
#define PROGRESS_NAME "progress"
#define VOLUME_NAME "volume"

#define PLANC
#if defined PLANA || defined PLANC
#define PIPE "uridecodebin expose-all-streams=false name=" SRC_NAME " \
caps=video/x-h264;audio/x-raw " SRC_NAME ". ! queue ! h264parse ! \
queue ! g1h264dec ! video/x-raw,width=%d,height=%d,format=BGRx ! \
progressreport silent=true do-query=true update-freq=1 format=time \
name=" PROGRESS_NAME " ! perf name=" PERF_NAME " ! \
g1kmssink gem-name=%d " SRC_NAME ". ! queue ! audioconvert ! \
audio/x-raw,format=S16LE ! volume name=" VOLUME_NAME " ! \
alsasink async=false enable-last-sample=false"
#else
#define PIPE "uridecodebin expose-all-streams=false name=" SRC_NAME " \
caps=video/x-h264;audio/x-raw " SRC_NAME ". ! queue ! h264parse ! \
queue ! g1h264dec ! video/x-raw,width=%2,height=%3 ! \
progressreport silent=true do-query=true update-freq=1 format=time \
name=" PROGRESS_NAME " ! perf name=" PERF_NAME " ! \
g1fbdevsink zero-memcpy=true max-lateness=-1 async=false \
enable-last-sample=false "
#endif

namespace mui
{

    gboolean
    VideoWindow::busCallback (GstBus *bus,
		 GstMessage *message,
		 gpointer    data)
    {
	VideoWindow *_this = (VideoWindow*)data;

	switch (GST_MESSAGE_TYPE (message))
	{
	case GST_MESSAGE_ERROR:
	{
	    GError *err;
	    gchar *debug;

	    gst_message_parse_error(message, &err, &debug);
	    cout << "error: " << err->message << endl;
	    g_error_free(err);
	    g_free(debug);

	    // g_main_loop_quit (loop);
	    break;
	}
	case GST_MESSAGE_EOS:
	{
	    /// end-of-stream
	    //emit _this->playState(5);
	    //_this->set_media("/opt/VideoWindow/media/Microchip-masters.mp4");
	    _this->set_media("/root/tst/video0.mp4");

	    // TODO: remove me, loop
	    _this->setState(GST_STATE_PLAYING);

	    break;
	}
	case GST_MESSAGE_ELEMENT:
	{
//#if defined PLANA || defined PLAND
	    const GstStructure *info = gst_message_get_structure(message);
	    if(gst_structure_has_name(info, PROGRESS_NAME))
	    {
		const GValue *vcurrent;
		gint64 current = 0;
		const GValue *vtotal;
		gint64 total = 0;

		vtotal = gst_structure_get_value (info, "total");
		total = g_value_get_int64(vtotal);
		vcurrent = gst_structure_get_value (info, "current");
		current = g_value_get_int64(vcurrent);

		//emit _this->positionChanged(current);
		//emit _this->durationChanged(total);

	    }
//#endif //PLANA
	    break;
	}
	case GST_MESSAGE_INFO:
	{
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


    VideoWindow::VideoWindow(const Size& size)
	: PlaneWindow(size, FLAG_WINDOW_DEFAULT | FLAG_NO_BACKGROUND, false),
	  m_video_pipeline(NULL),
	  m_src(NULL),
	  m_volume(NULL)
    {

	gst_init(NULL, NULL);


    }

    bool VideoWindow::createPipeline()
    {
	GError *error = NULL;
	GstBus *bus;
	//guint bus_watch_id;

	KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
	int _gem = screen->gem();

	/* Make sure we don't leave orphan references */
	destroyPipeline();

	char buffer[2048];
	sprintf(buffer, PIPE, w(), h(), _gem);

	printf("gem = %d w()= %d h()=%d \n", _gem, w(), h());

	string pipe(buffer);
	cout << pipe << endl;

	m_video_pipeline = gst_parse_launch (pipe.c_str(), &error);
	if (!m_video_pipeline)
	{
	    m_video_pipeline = NULL;
	    printf("failed to create video pipeline...\n");
	    return false;
	}

	m_src = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SRC_NAME);
	if (!m_src) {
	    m_src = NULL;
	    printf("failed to get video src element...\n");
	    return false;
	}

	m_volume = gst_bin_get_by_name(GST_BIN(m_video_pipeline), VOLUME_NAME);
	if (!m_volume) {
	    m_volume = NULL;
	    printf("failed to get volume element...\n");
	    return false;
	}

	bus = gst_pipeline_get_bus (GST_PIPELINE (m_video_pipeline));
	/*bus_watch_id =*/ gst_bus_add_watch (bus, &busCallback, this);
	gst_object_unref (bus);

	return true;
    }

    void VideoWindow::destroyPipeline()
    {
	if (m_video_pipeline)
	{
	    null();
	    g_object_unref(m_src);
	    g_object_unref(m_video_pipeline);
	    m_src = NULL;
	    m_video_pipeline = NULL;
	}
    }

    VideoWindow::~VideoWindow()
    {
	destroyPipeline();
    }

    bool VideoWindow::play(bool mute,int volume)
    {
	//emit playState(this->setState(GST_STATE_PLAYING));
	setState(GST_STATE_PLAYING);
	setVolume(volume);
	setMute(mute);

	return false;
    }

    bool VideoWindow::pause()
    {
	//emit playState(!this->setState(GST_STATE_PAUSED));
        setState(GST_STATE_PAUSED);
	return false;
    }

    bool VideoWindow::null()
    {
	return setState(GST_STATE_NULL);
    }

    bool VideoWindow::set_media(const string& uri)
    {
	destroyPipeline();
	createPipeline();
	g_object_set(m_src, "uri", (string("file://") + uri).c_str(), NULL);

	return true;
    }

    bool VideoWindow::setVolume(int volume)
    {
	if (!m_volume)
	    return false;

	g_object_set(m_volume, "volume", volume/100.0, NULL);
	return true;
    }

    bool VideoWindow::setMute(bool mute)
    {
	if (!m_volume)
	    return false;

	g_object_set(m_volume, "mute", mute, NULL);
	return true;
    }

    bool VideoWindow::setState(GstState state)
    {
	GstStateChangeReturn ret;

	if (m_video_pipeline)
	{
	    ret = gst_element_set_state(m_video_pipeline, state);
	    if (GST_STATE_CHANGE_FAILURE == ret)
	    {
		printf ("Unable to set video pipeline pipeline to %d!\n", state);
		return false;
	    }
	}
	else
	    return false;

	return true;
    }

}

#endif
