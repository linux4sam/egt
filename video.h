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
     * Video player window with hardware acceleration supported.
     */
    class HardwareVideo : public PlaneWindow
    {
    public:

	HardwareVideo(const Size& size);

	void draw();

	/**
	 * @brief Sets the media file URI to the current pipeline
	 * @param uri file URI
	 * @return true if success
	 */
	bool set_media(const std::string& uri);

	/**
	 * @brief Send pipeline to play state
	 * @return true if success
	 */
	bool play(bool mute = false,int volume = 100);
	bool unpause();

	/**
	 * @brief pause Send Pipeline to pause state
	 * @return true if success
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
	bool set_volume(int volume);

	int get_volume() const;

	/**
	 * @brief Mutes the audio of the video being played
	 * @param mute true if the audio is to be muted
	 * @return true if success
	 */
	bool set_mute(bool mute);

	virtual ~HardwareVideo();

	uint64_t position() const { return m_position; }
	uint64_t duration() const { return m_duration; }
    private:

	bool set_state(GstState state);
	bool createPipeline();
	void destroyPipeline();

	static gboolean busCallback(GstBus* bus,
				    GstMessage* message,
				    gpointer data);

	GstElement* m_video_pipeline;
	GstElement* m_src;
	GstElement* m_volume;
	gint64 m_position;
	gint64 m_duration;
	std::string m_filename;
    };

    /**
     * Video player window using only software.
     */
    class SoftwareVideo : public PlaneWindow
    {
    public:

	SoftwareVideo(const Size& size);

	void draw();

	/**
	 * @brief Sets the media file URI to the current pipeline
	 * @param uri file URI
	 * @return true if success
	 */
	bool set_media(const std::string& uri);

	/**
	 * @brief Send pipeline to play state
	 * @return true if success
	 */
	bool play(bool mute = false,int volume = 100);
	bool unpause();

	/**
	 * @brief pause Send Pipeline to pause state
	 * @return true if success
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
	bool set_volume(int volume);

	int get_volume() const;

	/**
	 * @brief Mutes the audio of the video being played
	 * @param mute true if the audio is to be muted
	 * @return true if success
	 */
	bool set_mute(bool mute);

	void scale(float value);
	float scale() const;

	virtual ~SoftwareVideo();

	uint64_t position() const { return m_position; }
	uint64_t duration() const { return m_duration; }

	bool playing() const;
    private:

	bool set_state(GstState state);
	bool createPipeline();
	void destroyPipeline();

	static gboolean busCallback(GstBus* bus,
				    GstMessage* message,
				    gpointer data);

	static GstFlowReturn on_new_buffer_from_source(GstElement* elt, gpointer data);

	GstElement* m_video_pipeline;
	GstElement* m_src;
	GstElement* m_volume;
	GstElement* m_testsink;
	gint64 m_position;
	gint64 m_duration;
	std::string m_filename;
	int m_volume_value;
	bool m_mute_value;
    };

}

#endif

#endif
