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

    AudioPlayer();

    /**
     * Construct and set the media file URI to the current pipeline.
     * @param uri file URI
     */
    AudioPlayer(const std::string& uri);

    /**
     * Sets the media file URI to the current pipeline
     * @param uri file URI
     * @return true if success
     */
    virtual bool set_media(const std::string& uri);

    /**
     * Send pipeline to play state
     * @return true if success
     */
    virtual bool play(bool mute = false, int volume = 100);

    /**
     * pause Send Pipeline to pause state
     * @return true if success
     */
    virtual bool pause();

    /**
     * unpause Send Pipeline to unpaused state
     * @return true if success
     */
    virtual bool unpause();

    /**
     * Adjusts the volume of the audio being played.
     *
     * @param volume desired volume in the range of 0 (no sound) to 100 (normal sound)
     * @return true if success
     */
    virtual bool set_volume(int volume);

    /**
     * Get the volume.
     */
    virtual int volume() const;

    /**
     * Mutes the audio being played.
     *
     * @param mute true if the audio is to be muted
     * @return true if success
     */
    virtual bool set_mute(bool mute);

    /**
     * Get the current position of the audio stream.
     */
    uint64_t position() const;

    /**
     * Get the duration of the audio stream.
     */
    uint64_t duration() const;

    /**
     * Seek to a position.
     *
     * The position is given by the position() function and the duration is
     * given by the duration() method.
     */
    virtual void seek(uint64_t pos);

    /**
     * Returns true if the stream is currently playing.
     */
    virtual bool playing() const;

    virtual ~AudioPlayer();

protected:

    /**
     * null Send pipeline to null state
     * @return true if success
     */
    bool null();

    /**
     * Create the pipeline.
     */
    virtual bool createPipeline();

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
