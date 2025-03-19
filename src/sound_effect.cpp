/*
 * Copyright (C) 2025 Microchip Technology Inc. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/sound_effect.h"

#include "detail/multimedia/sound_effect_impl.h"

namespace egt
{
inline namespace v1
{

SoundEffect::SoundEffect(const std::string& uri, const SoundEffect::SoundDevice& sound_device)
    : m_impl(std::make_unique<detail::SoundEffectImpl>(uri, sound_device))
{
}

SoundEffect::SoundEffect() noexcept
    : m_impl(std::make_unique<detail::SoundEffectImpl>())
{
}

SoundEffect::~SoundEffect() noexcept = default;

bool SoundEffect::device(const SoundEffect::SoundDevice& sound_device) noexcept
{
    return m_impl->device(sound_device);
}

const SoundEffect::SoundDevice& SoundEffect::device() const
{
    return m_impl->device();
}

bool SoundEffect::media(const std::string& uri) noexcept
{
    return m_impl->media(uri);
}

std::string SoundEffect::media() const
{
    return m_impl->media();
}

void SoundEffect::repeat(bool value)
{
    m_impl->repeat(value);
}

bool SoundEffect::repeat() const
{
    return m_impl->repeat();
}

void SoundEffect::play()
{
    m_impl->play();
}

void SoundEffect::stop()
{
    m_impl->stop();
}

std::vector<SoundEffect::SoundDevice> SoundEffect::sound_device_list()
{
    return detail::SoundEffectImpl::sound_device_list();
}

}
}
