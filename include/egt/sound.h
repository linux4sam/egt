/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SOUND_H
#define EGT_SOUND_H

/**
 * @file
 */

#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

namespace detail
{
struct soundimpl;
}

namespace experimental
{

/**
 * Simple class to manage playing raw or WAV PCM sound files.
 *
 * The default sound card is used by default, but another one can be specified.
 * @code{.cpp}
 * Sound sound("myfile.wav", "1:0")
 * @endcode
 *
 * Another way to configure the default sound card is at the system level.
 *
 * List available sound cards:
 * @code{.cpp}
 * cat /proc/asound/cards
 * @endcode
 *
 * Then, add the following to @b /etc/asound.conf
 * @code{.cpp}
 * defaults.pcm.card 1
 * defaults.ctl.card 1
 * @endcode
 */
class Sound
{
public:

    /**
     * @file The WAV file to play.
     * @param device ALSA sound device.
     */
    explicit Sound(const std::string& file,
                   const std::string& device = "default");

    /**
     * @file The WAV file to play.
     * @rate Rate of the sound file, i.e. 44100.
     * @channels The number of channels in the sound file.
     * @param device ALSA sound device.
     */
    explicit Sound(const std::string& file, unsigned int rate, int channels,
                   const std::string& device = "default");

    virtual void play(bool repeat = false);

    virtual ~Sound();

protected:

    void open_alsa_device(const std::string& device);
    void init_alsa_params(unsigned int rate, int channels);
    void open_file();

    std::unique_ptr<detail::soundimpl> m_impl;

    std::string m_file;
};

}
}
}

#endif
