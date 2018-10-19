/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_VIDEO_H
#define MUI_VIDEO_H

/**
 * @file
 * @brief Working with video output.
 */

#include "config.h"

#ifdef HAVE_GSTREAMER

#ifdef HAVE_LIBPLANES
#include <drm_fourcc.h>
#endif
#include <mui/window.h>
#include <gst/gst.h>
#include <string>

namespace mui
{
    inline namespace v1
    {
        /**
         * Hardware based video window.
         */
        class VideoWindow : public PlaneWindow
        {
        public:

            VideoWindow(const Size& size, uint32_t format = DRM_FORMAT_XRGB8888, bool heo = false);

            virtual void draw() override;

            /**
             * @brief Sets the media file URI to the current pipeline
             * @param uri file URI
             * @return true if success
             */
            virtual bool set_media(const std::string& uri);

            /**
             * @brief Send pipeline to play state
             * @return true if success
             */
            virtual bool play(bool mute = false, int volume = 100);
            virtual bool unpause();

            /**
             * @brief pause Send Pipeline to pause state
             * @return true if success
             */
            virtual bool pause();

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
            virtual bool set_volume(int volume);
            virtual int get_volume() const;

            /**
             * @brief Mutes the audio of the video being played
             * @param mute true if the audio is to be muted
             * @return true if success
             */
            virtual bool set_mute(bool mute);

            virtual void scale(float value);
            virtual float scale() const;

            virtual ~VideoWindow();

            inline uint64_t position() const
            {
                return m_position;
            }
            inline uint64_t duration() const
            {
                return m_duration;
            }

            virtual bool playing() const;

            virtual uint32_t fps() const
            {
                return m_fps;
            }

        protected:

            bool set_state(GstState state);
            virtual bool createPipeline() = 0;
            void destroyPipeline();

            static gboolean bus_callback(GstBus* bus,
                                         GstMessage* message,
                                         gpointer data);

            GstElement* m_video_pipeline {nullptr};
            GstElement* m_src {nullptr};
            GstElement* m_volume {nullptr};
            gint64 m_position {0};
            gint64 m_duration {0};
            std::string m_filename;
            int m_volume_value {100};
            uint32_t m_fps {0};

        private:


        };


        /**
         * Video player window with hardware acceleration supported.
         */
        class HardwareVideo : public VideoWindow
        {
        public:

            HardwareVideo(const Size& size, uint32_t format = DRM_FORMAT_YUYV);

            /**
             * @brief Sets the media file URI to the current pipeline
             * @param uri file URI
             * @return true if success
             */
            bool set_media(const std::string& uri) override;

            virtual ~HardwareVideo();

        protected:

            virtual bool createPipeline() override;
        };

        /**
         * Video player window using only software.
         */
        class SoftwareVideo : public VideoWindow
        {
        public:

            SoftwareVideo(const Size& size, uint32_t format = DRM_FORMAT_YUV420);

            virtual bool set_media(const std::string& uri) override;

            virtual ~SoftwareVideo();

        protected:

            virtual bool createPipeline() override;
            void destroyPipeline();

            static GstFlowReturn on_new_buffer_from_source(GstElement* elt,
                    gpointer data);

            GstElement* m_appsink;
        };

        /**
         * Specialized SoftwareVideo window with support for a V4L2 source.
         */
        class V4L2SoftwareVideo : public SoftwareVideo
        {
        public:

            explicit V4L2SoftwareVideo(const Size& size);

            virtual bool set_media(const std::string& uri) override;

            virtual ~V4L2SoftwareVideo();

        protected:

            virtual bool createPipeline() override;
        };

        /**
         * Specialized HardwareVideo window with support for a V4L2 source.
         */
        class V4L2HardwareVideo : public HardwareVideo
        {
        public:

            explicit V4L2HardwareVideo(const Size& size);

            virtual bool set_media(const std::string& uri) override;

            virtual ~V4L2HardwareVideo();

        protected:

            virtual bool createPipeline() override;
        };

        /**
         * Specialized SoftwareVideo window that only handles RAW video stream sources.
         */
        class RawSoftwareVideo : public SoftwareVideo
        {
        public:

            explicit RawSoftwareVideo(const Size& size);

            virtual bool set_media(const std::string& uri) override;

            virtual ~RawSoftwareVideo();

        protected:

            virtual bool createPipeline() override;
        };

    }
}

#endif

#endif
