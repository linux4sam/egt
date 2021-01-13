/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VIDEO_H
#define EGT_VIDEO_H

/**
 * @file
 * @brief Working with video.
 */

#include <egt/detail/meta.h>
#include <egt/signal.h>
#include <egt/window.h>

namespace egt
{
inline namespace v1
{
namespace detail
{
/// @todo these functions should be internal
/// @private
bool is_target_sama5d4();
/// @private
bool audio_device();
/// @private
WindowHint check_windowhint(WindowHint& hint);
class GstDecoderImpl;
class GstKmsSinkImpl;
class GstAppSinkImpl;
}

/**
 * A VideoWindow is a widget to decode video and render it to a screen.
 *
 * It has a bounding rectangle, format and WindowHint. These properties can
 * be manipulated to create a video window either as a basic window or an
 * overlay plane.
 *
 * The video decoding is done through gstreamer media framework.
 *
 * @ingroup media
 */
class EGT_API VideoWindow : public Window
{
public:

    /**
     * Event signal.
     * @{
     */
    /// Invoked when the position of the player changes.
    Signal<int64_t> on_position_changed;

    /// Invoked when an error occurs.
    Signal<const std::string&> on_error;

    /// Invoked on end of stream.
    Signal<> on_eos;

    /// Invoked when the state of the player changes.
    Signal<> on_state_changed;
    /** @} */

    /**
     * Create a video window to decode video and render it to a screen.
     *
     * @param[in] rect Initial rectangle of the widget.
     * @param[in] format Pixel format of window or a overlay plane.
     * @param[in] hint Used for configuring window backends.
     *
     * @note Only WindowHint::heo_overlay can use yuyv, nv21 and yuv420 pixel
     * formats.
     */
    explicit VideoWindow(const Rect& rect = {},
                         PixelFormat format = PixelFormat::xrgb8888,
                         WindowHint hint = WindowHint::overlay);

    /**
    * Create a video window to decode video and render it to a screen.
    *
    * @param[in] rect Initial rectangle of the widget.
    * @param[in] uri Media file
    * @param[in] format Pixel format of window or a overlay plane.
    * @param[in] hint Used for configuring window backends.
    *
    * @note Only WindowHint::heo_overlay can use yuyv, nv21 and yuv420 pixel
    * formats.
    */
    VideoWindow(const Rect& rect,
                const std::string& uri,
                PixelFormat format = PixelFormat::xrgb8888,
                WindowHint hint = WindowHint::overlay);

    VideoWindow(const VideoWindow&) = delete;
    VideoWindow& operator=(const VideoWindow&) = delete;
    VideoWindow(VideoWindow&&) noexcept;
    VideoWindow& operator=(VideoWindow&&) noexcept;

    void do_draw() override
    {
        // video windows don't draw
    }

    void draw(Painter& painter, const Rect& rect) override;

    /**
     * Initialize gstreamer pipeline for specified media file.
     *
     * @param uri Media file
     * @return true on success
     */
    bool media(const std::string& uri);

    /**
     * get the media file used of video playback.
     *
     * @return media file.
     */
    EGT_NODISCARD std::string media() const
    {
        return m_uri;
    }

    /**
     * Play the video.
     *
     * @return true on success
     */
    bool play();

    /**
     * Pause the video.
     *
     * @return true on success
     */
    bool pause();

    /**
     * Check is video in play state.
     *
     * @return true on success
     */
    EGT_NODISCARD bool playing() const;

    /**
     * Get the current position of the video being played.
     *
     * @return Time duration in nanoseconds.
     */
    EGT_NODISCARD int64_t position() const;

    /**
     * Get the total duration of the video.
     *
     * @return Time duration in nanoseconds.
     */
    EGT_NODISCARD int64_t duration() const;

    /**
     * Adjust volume of the video being played.
     *
     * @param volume Value in the range 0 - 100.
     * @return true on success
     */
    bool volume(int volume);

    /**
     * Get the volume value for the video being played.
     * @return Value in the range 0 - 100.
     */
    EGT_NODISCARD int volume() const;

    /**
     * Seek to a position.
     *
     * The position is given by the position() function and the duration is
     * given by the duration() method.
     *
     * @param pos Position in nanoseconds.
     * @return true on success
     */
    bool seek(int64_t pos);

    /**
     * Enable/disable continues loop-back mode of the video
     * being played. by default this is disabled.
     *
     * @param enable enable/disable loop-back mode.
     */
    void loopback(bool enable)
    {
        m_loopback = enable;
    }

    /**
     * Get loop-back state
     *
     * @return true/false based on loop-back state
     */
    EGT_NODISCARD bool loopback() const
    {
        return m_loopback;
    }

    using Window::scale;
    void scale(float hscale, float vscale) override;

    using Window::resize;
    void resize(const Size& s) override;

    /**
     * Get horizontal scale value.
     */
    EGT_NODISCARD float hscale() const
    {
        return m_hscale;
    }

    /**
     * Get vertical scale value.
     */
    EGT_NODISCARD float vscale() const
    {
        return m_vscale;
    }

    /**
     * check for audio is supported. check is done based on
     * sound device and audio track present in media file.
     *
     * @return true if supported and false if not supported.
     */
    EGT_NODISCARD bool has_audio() const;

    void serialize(Serializer& serializer) const override;

    void deserialize(const std::string& name, const std::string& value,
                     const Serializer::Attributes& attrs) override;

    ~VideoWindow() noexcept override;

protected:
    /// Loopback enabled.
    bool m_loopback{false};

    /// Horizontal scale value.
    float m_hscale{1.0};

    /// Vertical scale value.
    float m_vscale{1.0};

    /// media file
    std::string m_uri;

    /// Create the internal implementation.
    void create_impl(const Size& size);

    /// @private
    std::unique_ptr<detail::GstDecoderImpl> m_video_impl;

    friend class detail::GstDecoderImpl;
    friend class detail::GstKmsSinkImpl;
    friend class detail::GstAppSinkImpl;
};

}

}

#endif
