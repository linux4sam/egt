/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef VIDEO_H
#define VIDEO_H

#ifdef HAVE_GSTREAMER

#include "window.h"
#include <gst/gst.h>
#include <string>

namespace mui
{

    /**
     *
     */
    class VideoWindow : public PlaneWindow
    {
    public:

	VideoWindow(const Size& size);

	/**
	 * @brief Sets the media file URI to the current pipeline
	 * @param uri file URI
	 * @return true if success
	 */
	bool set_media(const std::string& uri);

	//void durationChanged(int64_t duration);      // Media Duration changed
	//void positionChanged(int64_t position);      // Media position changed
	//void playState(int state);                 // pipeline play state (true = onPlay, false = onPause)

	/**
	 * @brief play Send pipeline to play state
	 * @return  true if success
	 */
	bool play(bool mute = false,int volume = 100);

	/**
	 * @brief pause Send Pipeline to pause state
	 * @return true is success
	 */
	bool pause();

	/**
	 * @brief null Send pipeline to null state
	 * @return true if success
	 */
	bool null();

	/**
	 * @brief Adjusts the volume of the audio in the video being played
	 * @param volume desired volume in the range of 0 (no sound) to 100 (normal sound)
	 * @return true if success
	 */
	bool setVolume(int volume);

	/**
	 * @brief Mutes the audio of the video being played
	 * @param mute true if the audio is to be muted
	 * @return true if success
	 */
	bool setMute(bool mute);

	virtual ~VideoWindow();

private:

	// not supported
	void add(Widget* widget) {}

	bool setState(GstState state);
	bool createPipeline();
	void destroyPipeline();

	static gboolean busCallback(GstBus* bus,
				    GstMessage* message,
				    gpointer data);

	GstElement* m_video_pipeline;
	GstElement* m_src;
	GstElement* m_volume;
    };

}

#endif

#endif
