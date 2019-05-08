/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VIDEOVIEW_H
#define EGT_VIDEOVIEW_H

#include <egt/detail/video/gstdecoderimpl.h>
#include <egt/window.h>

namespace egt
{
inline namespace v1
{
namespace detail
{
void init_gst_thread();
bool is_target_sama5d4();
class GstKmsSinkImpl;
class GstAppSinkImpl;
}

class VideoWindow : public Window
{
public:
    VideoWindow(const Size& size,
                pixel_format format = pixel_format::xrgb8888,
                windowhint hint = windowhint::overlay);

    VideoWindow(const Rect& rect,
                pixel_format format = pixel_format::xrgb8888,
                windowhint hint = windowhint::overlay);

    virtual void top_draw() override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Sets the media file to the current pipeline
     * @param filename of a media file
     * @return true if success
     */
    bool set_media(const std::string& filename);

    /**
     * Send pipeline to play state
     * @return true if success
     */
    bool play();

    /**
     * pause Send Pipeline to pause state
     * @return true if success
     */
    bool pause();

    /**
     * playing check if Pipeline is in play state
     * @return true if success
     */
    virtual bool playing() const;

    /**
     * position gets the current position of video getting played
     * @return 64bit time
     */
    uint64_t position();

    /**
     * duration gets the duration of video getting played
     * @return 64bit time
     */
    uint64_t duration();

    void move(const Point& point);

    /**
     * Adjusts the volume of the audio in the video being played
     * @param volume desired volume in the range of 0 (no sound) to 10 (normal sound)
     * @return true if success
     */
    bool set_volume(double volume);

    /**
     * gets the volume of the audio in the video being played
     * @return volume value set for video being played
     */
    double get_volume();

    /**
     * gets the scale value
     */
    float scale();

    /**
     * sets the scale value
     */
    void set_scale(float value);

    /**
     * seek to time of the video being played
     * @param time in nanoseconds
     * @return true if success
     */
    bool seek(int64_t time_nanoseconds);

    /**
     * play video in loop-back
     * @param bool to enable/disable loop-back
     */
    void set_loopback(bool enable = false)
    {
        m_loopback = enable;
    }

    /**
     * get loop-back state
     * @return true/false based on set_loopback.
     */
    bool get_loopback()
    {
        return m_loopback;
    }

    void draw_frame(GstSample* sample)
    {
        m_decoderImpl->push_buffer(sample);
        Window::damage();
    }

    /**
     * get Error Message received from pipeline
     */
    std::string get_error_message()
    {
        return m_decoderImpl->get_error_message();
    }

    void handle_gst_events(detail::gsteventid event);

    virtual ~VideoWindow();

protected:
    std::string m_filename;
    void createImpl(const Size& size);
    bool is_target_sama5d4();
    bool get_media_info(const std::string& filename);
    void set_media_info(const std::string& info, const int type);

    static void print_stream_info(GstDiscovererStreamInfo* info, VideoWindow* data);
    static void print_topology(GstDiscovererStreamInfo* info, VideoWindow* data);
    static void on_discovered_cb(GstDiscoverer* discoverer, GstDiscovererInfo* info, gpointer data);
    static void on_finished_cb(GstDiscoverer* discoverer, gpointer data);

private:
    std::shared_ptr<detail::GstDecoderImpl> m_decoderImpl;
    bool m_seekable;
    std::string m_vcodec;
    std::string m_acodec;
    std::string m_ctype;
    bool m_atrack;
    bool m_vtrack;
    bool m_loopback;
    GMainLoop* m_discoverer_loop;
    std::string m_audio_info;
    std::string m_video_info;

    friend class detail::GstDecoderImpl;
    friend class detail::GstKmsSinkImpl;
    friend class detail::GstAppSinkImpl;

    VideoWindow() = delete;
};

} //namespace v1

} //namespace egt

#endif
