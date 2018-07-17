/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "video.h"
#include "kmsscreen.h"
#include <thread>

#ifdef HAVE_GSTREAMER

#include <gst/app/gstappsink.h>

using namespace std;

#define SRC_NAME "srcVideo"
#define PERF_NAME "perf"
#define PROGRESS_NAME "progress"
#define VOLUME_NAME "volume"
#define SINK_NAME "testsink"

static void init_thread()
{
    static bool init = false;
    if (!init)
    {
	static std::thread t([](){
		GMainLoop *mainloop = g_main_loop_new(NULL, FALSE);
		g_main_loop_run(mainloop);
	    });
	init = true;
    }
}

namespace mui
{

#define HARDWAREPIPE "uridecodebin expose-all-streams=false name=" SRC_NAME " \
caps=video/x-h264;audio/x-raw " SRC_NAME ". ! queue ! h264parse !	\
queue ! g1h264dec ! video/x-raw,width=%d,height=%d,format=BGRx !	\
progressreport silent=true do-query=true update-freq=1 format=time	\
name=" PROGRESS_NAME " ! perf name=" PERF_NAME " !			\
g1kmssink gem-name=%d " SRC_NAME ". ! queue ! audioconvert !		\
audio/x-raw,format=S16LE ! volume name=" VOLUME_NAME " !		\
alsasink async=false enable-last-sample=false"

    gboolean HardwareVideo::busCallback(GstBus* bus, GstMessage* message, gpointer data)
    {
	HardwareVideo *_this = (HardwareVideo*)data;

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
	    _this->set_media(_this->m_filename);

	    // TODO: remove me, loop
	    _this->set_state(GST_STATE_PLAYING);

	    break;
	}
	case GST_MESSAGE_ELEMENT:
	{
	    const GstStructure *info = gst_message_get_structure(message);
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

    HardwareVideo::HardwareVideo(const Size& size)
	: PlaneWindow(size, FLAG_WINDOW_DEFAULT | FLAG_NO_BACKGROUND, DRM_FORMAT_XRGB8888),
	  m_video_pipeline(NULL),
	  m_src(NULL),
	  m_volume(NULL),
	  m_position(0),
	  m_duration(0)
    {
	gst_init(NULL, NULL);
	init_thread();
    }

    void HardwareVideo::draw()
    {
	// don't do any drawing to a window because we don't support video formats, only positioning
	KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(m_screen);
	s->apply();
	m_dirty = false;
	m_damage.clear();
    }

    bool HardwareVideo::createPipeline()
    {
	GError *error = NULL;
	GstBus *bus;
	//guint bus_watch_id;

	KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
	int _gem = screen->gem();

	/* Make sure we don't leave orphan references */
	destroyPipeline();

	char buffer[2048];
	sprintf(buffer, HARDWAREPIPE, w(), h(), _gem);

	DBG("gem = " << _gem << " w()=" << w() << " h()=" << h());

	string pipe(buffer);
	cout << pipe << endl;

	m_video_pipeline = gst_parse_launch (pipe.c_str(), &error);
	if (!m_video_pipeline)
	{
	    m_video_pipeline = NULL;
	    ERR("failed to create video pipeline");
	    return false;
	}

	m_src = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SRC_NAME);
	if (!m_src) {
	    m_src = NULL;
	    ERR("failed to get video src element");
	    return false;
	}

	m_volume = gst_bin_get_by_name(GST_BIN(m_video_pipeline), VOLUME_NAME);
	if (!m_volume) {
	    m_volume = NULL;
	    ERR("failed to get volume element");
	    return false;
	}

	bus = gst_pipeline_get_bus (GST_PIPELINE (m_video_pipeline));
	/*bus_watch_id =*/ gst_bus_add_watch (bus, &busCallback, this);
	gst_object_unref (bus);

	return true;
    }

    void HardwareVideo::destroyPipeline()
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

    HardwareVideo::~HardwareVideo()
    {
	destroyPipeline();
    }

    bool HardwareVideo::play(bool mute,int volume)
    {
	set_state(GST_STATE_PLAYING);
	return false;
    }

    bool HardwareVideo::unpause()
    {
	set_state(GST_STATE_PLAYING);
	return false;
    }

    bool HardwareVideo::pause()
    {
        set_state(GST_STATE_PAUSED);
	return false;
    }

    bool HardwareVideo::null()
    {
	return set_state(GST_STATE_NULL);
    }

    bool HardwareVideo::set_media(const string& uri)
    {
	m_filename = uri;

	destroyPipeline();
	createPipeline();
	g_object_set(m_src, "uri", (string("file://") + uri).c_str(), NULL);

	return true;
    }

    bool HardwareVideo::set_volume(int volume)
    {
	if (!m_volume)
	    return false;

	if (volume < 1)
	    volume = 1;
	if (volume > 100)
	    volume = 100;

	g_object_set(m_volume, "volume", volume/100.0, NULL);
	invoke_handlers();

	return true;
    }

    int HardwareVideo::get_volume() const
    {
	if (!m_volume)
	    return false;

	gdouble volume = 0;
	g_object_get(m_volume, "volume", &volume, NULL);

	return volume * 100.0;
    }

    bool HardwareVideo::set_mute(bool mute)
    {
	if (!m_volume)
	    return false;

	g_object_set(m_volume, "mute", mute, NULL);
	invoke_handlers();

	return true;
    }

    bool HardwareVideo::set_state(GstState state)
    {
	GstStateChangeReturn ret;

	if (m_video_pipeline)
	{
	    ret = gst_element_set_state(m_video_pipeline, state);
	    if (GST_STATE_CHANGE_FAILURE == ret)
	    {
		ERR("unable to set video pipeline pipeline to " << state);
		return false;
	    }

	    invoke_handlers();
	}
	else
	    return false;

	return true;
    }


    /*
      #define PIPE2 "uridecodebin expose-all-streams=false name=" SRC_NAME " \
      caps=video/x-h264;audio/x-raw " SRC_NAME ". ! queue ! h264parse ! \
      queue ! avdec_h264 ! videoconvert ! video/x-raw,width=%d,height=%d,format=BGRx ! \
      progressreport silent=true do-query=true update-freq=1 format=time \
      name=" PROGRESS_NAME " ! \
      appsink caps=\"video/x-raw\" name=" SINK_NAME " ! queue ! audioconvert ! \
      audio/x-raw,format=S16LE ! volume name=" VOLUME_NAME " ! \
      alsasink async=false enable-last-sample=false"
    */

    /*
      #define PIPE2 "uridecodebin expose-all-streams=false name=" SRC_NAME " \
      caps=video/x-h263;audio/x-raw " SRC_NAME ". ! queue ! h263parse ! \
      queue ! avdec_h263 ! videoconvert ! video/x-raw,width=%d,height=%d,format=BGRx ! \
      progressreport silent=true do-query=true update-freq=1 format=time \
      name=" PROGRESS_NAME " ! \
      appsink wait-on-eos=true caps=\"video/x-raw\" name=" SINK_NAME " ! queue ! audioconvert ! \
      audio/x-raw,format=S16LE ! volume name=" VOLUME_NAME " ! \
      alsasink async=false enable-last-sample=false"
    */

/*
  #define FORMAT "h263"
  #define PIPE2 "uridecodebin expose-all-streams=true name=" SRC_NAME " \
  caps=video/x-" FORMAT ";audio/x-raw " \
  SRC_NAME ". ! queue ! " FORMAT "parse ! \
  avdec_" FORMAT " ! videoconvert ! video/x-raw,width=%d,height=%d,format=YUYV ! \
  progressreport silent=true do-query=true update-freq=1 format=time name=" PROGRESS_NAME " ! \
  appsink drop=true enable-last-sample=false caps=\"video/x-raw;audio/x-raw\" name=" SINK_NAME " " \
  SRC_NAME ". ! queue ! audioconvert ! audio/x-raw,format=S16LE ! volume name=" VOLUME_NAME " ! \
  alsasink async=false enable-last-sample=false"
*/

    /*
      #define PIPE2 "filesrc name=" SRC_NAME " ! videoparse width=%d height=%d framerate=24/1 format=4 ! autovideoconvert ! \
      progressreport silent=true do-query=true update-freq=1 format=time name=" PROGRESS_NAME " ! \
      appsink drop=true enable-last-sample=false caps=\"video/x-raw;audio/x-raw\" name=" SINK_NAME
    */

#define SOFTWAREPIPE "uridecodebin expose-all-streams=false name=" SRC_NAME " caps=video/x-raw;audio/x-raw " \
    SRC_NAME ". ! queue ! autovideoconvert ! "				\
    "progressreport silent=true do-query=true update-freq=1 format=time name=" PROGRESS_NAME " ! " \
    "appsink drop=true enable-last-sample=false caps=\"video/x-raw;audio/x-raw\" name=" SINK_NAME " " \
    SRC_NAME ". ! queue ! audioconvert ! volume name=" VOLUME_NAME " ! " \
    "alsasink async=false enable-last-sample=false sync=true"

    gboolean SoftwareVideo::busCallback(GstBus* bus, GstMessage* message, gpointer data)
    {
	SoftwareVideo *_this = (SoftwareVideo*)data;

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
	/*
	case GST_MESSAGE_WARNING:
	{
	    GError *err;
	    gchar *debug;

	    gst_message_parse_error(message, &err, &debug);
	    cout << "warning: " << err->message << endl;
	    g_error_free(err);
	    g_free(debug);

	    // g_main_loop_quit (loop);
	    break;
	}
	*/
	/*
	  case GST_MESSAGE_INFO:
	  {
	  GError *err;
	  gchar *debug;

	  gst_message_parse_error(message, &err, &debug);
	  cout << "info: " << err->message << endl;
	  g_error_free(err);
	  g_free(debug);

	  // g_main_loop_quit (loop);
	  break;
	  }
	*/
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
	    // TODO: remove me, loop
	    gst_element_seek (_this->m_video_pipeline, 1.0, GST_FORMAT_TIME,
			      GST_SEEK_FLAG_FLUSH,
	    		      GST_SEEK_TYPE_SET, 0,
	    		      GST_SEEK_TYPE_NONE, -1);

	    _this->set_state(GST_STATE_PLAYING);

	    break;
	}
	case GST_MESSAGE_ELEMENT:
	{
	    const GstStructure *info = gst_message_get_structure(message);
	    if(gst_structure_has_name(info, PROGRESS_NAME))
	    {
		const GValue *vcurrent;
		const GValue *vtotal;

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

    GstFlowReturn SoftwareVideo::on_new_buffer_from_source(GstElement * elt, gpointer data)
    {
	SoftwareVideo *_this = (SoftwareVideo*)data;
	GstSample* sample;

	g_signal_emit_by_name(elt, "pull-sample", &sample);

	if (sample)
	{
	    GstBuffer* buffer = gst_sample_get_buffer(sample);
	    GstMapInfo map;

	    if (gst_buffer_map(buffer, &map, GST_MAP_READ))
	    {
		KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(_this->m_screen);
		memcpy(screen->raw(), map.data, map.size);
		gst_buffer_unmap(buffer, &map);
	    }

	    gst_sample_unref(sample);
	}

	return GST_FLOW_OK;
    }

    SoftwareVideo::SoftwareVideo(const Size& size)
	: PlaneWindow(size, FLAG_WINDOW_DEFAULT | FLAG_NO_BACKGROUND,
		      DRM_FORMAT_YUYV),
	  m_video_pipeline(NULL),
	  m_src(NULL),
	  m_volume(NULL),
	  m_position(0),
	  m_duration(0)
    {
	gst_init(NULL, NULL);
	init_thread();
    }

    void SoftwareVideo::scale(float value)
    {
	KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
	screen->scale(value);
    }

    float SoftwareVideo::scale() const
    {
	KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
	return screen->scale();
    }

    void SoftwareVideo::draw()
    {
	// don't do any drawing to a window because we don't support video formats, only positioning
	KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(m_screen);
	s->apply();
	m_dirty = false;
	m_damage.clear();
    }

    bool SoftwareVideo::createPipeline()
    {
	GError *error = NULL;
	GstBus *bus;
	//guint bus_watch_id;

	/* Make sure we don't leave orphan references */
	destroyPipeline();

	char buffer[2048];
	sprintf(buffer, SOFTWAREPIPE);

	string pipe(buffer);
	cout << pipe << endl;

	m_video_pipeline = gst_parse_launch (pipe.c_str(), &error);
	if (!m_video_pipeline)
	{
	    m_video_pipeline = NULL;
	    ERR("failed to create video pipeline");
	    return false;
	}

	m_src = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SRC_NAME);
	if (!m_src) {
	    m_src = NULL;
	    ERR("failed to get video src element");
	    return false;
	}

	m_volume = gst_bin_get_by_name(GST_BIN(m_video_pipeline), VOLUME_NAME);
	if (!m_volume) {
	    ERR("failed to get volume element");
	}

	m_testsink = gst_bin_get_by_name(GST_BIN(m_video_pipeline), SINK_NAME);
	if (!m_testsink) {
	    ERR("failed to get test sink element");
	    return false;
	}

	g_object_set(G_OBJECT(m_testsink), "emit-signals", TRUE, "sync", TRUE, NULL);
	g_signal_connect(m_testsink, "new-sample",
			 G_CALLBACK(on_new_buffer_from_source), this);


	bus = gst_pipeline_get_bus (GST_PIPELINE (m_video_pipeline));
	/*bus_watch_id =*/ gst_bus_add_watch (bus, &busCallback, this);
	gst_object_unref (bus);

	return true;
    }

    void SoftwareVideo::destroyPipeline()
    {
	if (m_video_pipeline)
	{
	    null();
	    g_object_unref(m_src);
	    g_object_unref(m_testsink);
	    g_object_unref(m_video_pipeline);
	    m_src = NULL;
	    m_video_pipeline = NULL;
	}
    }

    SoftwareVideo::~SoftwareVideo()
    {
	destroyPipeline();
    }

    bool SoftwareVideo::play(bool mute, int volume)
    {
	set_state(GST_STATE_PLAYING);
	return false;
    }

    bool SoftwareVideo::unpause()
    {
	set_state(GST_STATE_PLAYING);
	return false;
    }

    bool SoftwareVideo::pause()
    {
        set_state(GST_STATE_PAUSED);
	return false;
    }

    bool SoftwareVideo::null()
    {
	return set_state(GST_STATE_NULL);
    }

    bool SoftwareVideo::set_media(const string& uri)
    {
	m_filename = uri;

	destroyPipeline();
	createPipeline();
	g_object_set(m_src, "uri", (string("file://") + uri).c_str(), NULL);

	return true;
    }

    bool SoftwareVideo::set_volume(int volume)
    {
	assert(m_volume);
	if (!m_volume)
	    return false;

	if (volume < 1)
	    volume = 1;
	if (volume > 100)
	    volume = 100;

	g_object_set(m_volume, "volume", volume/100.0, NULL);
	invoke_handlers();

	return true;
    }

    int SoftwareVideo::get_volume() const
    {
	if (!m_volume)
	    return 0;

	gdouble volume = 0;
	g_object_get(m_volume, "volume", &volume, NULL);
	return volume * 100.0;
    }

    bool SoftwareVideo::set_mute(bool mute)
    {
	if (!m_volume)
	    return false;

	g_object_set(m_volume, "mute", mute, NULL);
	invoke_handlers();
	return true;
    }

    bool SoftwareVideo::set_state(GstState state)
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

    bool SoftwareVideo::playing() const
    {
	GstState state;
	GstStateChangeReturn ret;

	if (m_video_pipeline)
	{
	    ret = gst_element_get_state(m_video_pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
	    return state == GST_STATE_PLAYING;
	}

	return false;
    }

}

#endif
