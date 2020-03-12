/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_AUDIO_H
#define EGT_AUDIO_H

/**
 * @file
 * @brief Working with audio.
 */

/**
 * @defgroup media Images and Video
 * Image and video related classes.
 */

#include <egt/detail/meta.h>
#include <egt/detail/object.h>
#include <egt/detail/signal.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

namespace detail
{
struct AudioPlayerImpl;
}

/**
 * Audio player.
 *
 * @ingroup media
 */
class EGT_API AudioPlayer : public detail::Object
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when the position of the player changes.
     */
    detail::Signal<> on_position_changed;

    /**
     * Invoked when an error occurs.
     */
    detail::Signal<const std::string&> on_error;

    /**
     * Invoked on end of stream.
     */
    detail::Signal<> on_eos;

    /**
     * Invoked when the state of the player changes.
     */
    detail::Signal<> on_state_changed;
    /** @} */

    AudioPlayer();

    /**
     * Construct and set the media file URI to the current pipeline.
     * @param uri file URI
     */
    explicit AudioPlayer(const std::string& uri);

    /**
     * Sets the media file URI to the current pipeline
     * @param uri file URI
     * @return true on success
     */
    virtual bool media(const std::string& uri);

    /**
     * Send pipeline to play state
     * @return true on success
     */
    virtual bool play();

    /**
     * pause Send Pipeline to pause state
     * @return true on success
     */
    virtual bool pause();

    /**
     * unpause Send Pipeline to unpaused state
     * @return true on success
     */
    virtual bool unpause();

    /**
     * Adjusts the volume of the audio being played.
     *
     * @param volume Value in the range 0 - 100.
     * @return true on success
     */
    virtual bool volume(int volume);

    /**
     * Get the volume.
     * @return Value in the range 0 - 100.
     */
    virtual int volume() const;

    /**
     * Mutes the audio being played.
     *
     * @param mute When true, audio is to be muted.
     * @return true on success
     */
    virtual bool mute(bool mute);

    /**
     * Get the current position of the audio stream.
     *
     * @return Time duration in nanoseconds.
     */
    uint64_t position() const;

    /**
     * Get the duration of the audio stream.
     *
     * @return Time duration in nanoseconds.
     */
    uint64_t duration() const;

    /**
     * Seek to a position.
     *
     * The position is given by the position() function and the duration is
     * given by the duration() method.
     *
     * @param pos Position in nanoseconds.
     * @return true on success
     */
    virtual bool seek(uint64_t pos);

    /**
     * Returns true if the stream is currently playing.
     */
    virtual bool playing() const;

    virtual ~AudioPlayer();

protected:

    /**
     * Send pipeline to null state
     * @return true on success
     */
    bool null();

    /**
     * Create the pipeline.
     */
    virtual bool createPipeline(const std::string& uri);

    /**
     * Destroy the pipeline.
     */
    void destroyPipeline();

    /**
     * Implementation pointer.
     */
    std::unique_ptr<detail::AudioPlayerImpl> m_impl;
};

}
}

#endif
