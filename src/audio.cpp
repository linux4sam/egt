/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "detail/multimedia/gstmeta.h"
#include "detail/multimedia/gstdecoderimpl.h"
#include "egt/app.h"
#include "egt/audio.h"
#include "egt/detail/filesystem.h"
#include "egt/detail/meta.h"
#include "egt/respath.h"
#include "egt/video.h"
#include <exception>
#include <gst/gst.h>
#include <sstream>
#include <thread>

namespace egt
{
inline namespace v1
{

AudioPlayer::AudioPlayer()
    : m_impl(std::make_unique<detail::GstDecoderImpl>(nullptr, Size{})),
      on_position_changed(&m_impl->on_position_changed),
      on_error(&m_impl->on_error),
      on_eos(&m_impl->on_eos),
      on_state_changed(&m_impl->on_state_changed)
{
    if (!detail::audio_device())
        throw std::runtime_error("no sound cards");
}

AudioPlayer::AudioPlayer(const std::string& uri)
    : AudioPlayer()
{
    if (!media(uri))
    {
        throw std::runtime_error("failed to initialize gstreamer pipeline");
    }
}

AudioPlayer::~AudioPlayer() noexcept
{
}

bool AudioPlayer::play()
{
    return m_impl->play();
}

bool AudioPlayer::pause()
{
    return m_impl->pause();
}

bool AudioPlayer::media(const std::string& uri)
{
    return m_impl->media(uri);
}

bool AudioPlayer::volume(int volume)
{
    return m_impl->volume(volume);
}

int AudioPlayer::volume() const
{
    return m_impl->volume();
}

bool AudioPlayer::mute(bool mute)
{
    return m_impl->mute(mute);
}

uint64_t AudioPlayer::position() const
{
    return m_impl->position();
}

uint64_t AudioPlayer::duration() const
{
    return m_impl->duration();
}

bool AudioPlayer::playing() const
{
    return m_impl->playing();
}

inline uint64_t sec_to_nsec(uint64_t s)
{
    return s * 1000000000ULL;
}

bool AudioPlayer::seek(uint64_t pos)
{
    return m_impl->seek(pos);
}

}
}
