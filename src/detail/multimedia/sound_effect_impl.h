/*
 * Copyright (C) 2025 Microchip Technology Inc. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_SOUND_EFFECT_IMPL_H
#define EGT_SRC_DETAIL_SOUND_EFFECT_IMPL_H

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <alsa/asoundlib.h>
#include <sndfile.hh>

#include "egt/sound_effect.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

class SoundEffectImpl final
{
public:

    explicit SoundEffectImpl(const std::string& uri, const SoundEffect::SoundDevice& sound_device = {});

    explicit SoundEffectImpl();

    SoundEffectImpl(const SoundEffectImpl&) = delete;

    SoundEffectImpl& operator=(const SoundEffectImpl&) = delete;

    ~SoundEffectImpl() noexcept;

    bool device(const SoundEffect::SoundDevice& device) noexcept;

    const SoundEffect::SoundDevice& device() const
    {
        return m_sound_device;
    }

    bool media(const std::string& uri) noexcept;

    std::string media() const
    {
        return m_uri;
    }

    void repeat(bool value)
    {
        m_repeat = value;
    }

    bool repeat() const
    {
        return m_repeat;
    }

    void play();

    void stop();

    static std::vector<SoundEffect::SoundDevice> sound_device_list();

private:

    bool open_pcm_device();
    bool open_file(const std::string& path);
    bool configure_pcm_device();

    void playback();

    static std::once_flag init_flag;
    static int ipc_key;
    static bool init();
    static bool create_plugins(int card, int dev);

    static constexpr snd_pcm_format_t to_pcm_format(int format);
    static std::string string_of_major_format(int format);
    static std::string string_of_minor_format(int format);
    static std::string string_of_endianness_format(int format);
    static void dump_alsa_conf(snd_config_t* config, int offset);

    SoundEffect::SoundDevice m_sound_device{};
    std::string m_uri{};

    snd_pcm_t* m_snd_pcm_handle{nullptr};
    std::unique_ptr<SndfileHandle> m_sndfile_handle;
    int m_format{0};
    int m_channels{0};
    unsigned int m_samplerate{0};
    snd_pcm_uframes_t m_period_size;
    snd_pcm_format_t m_pcm_format;

    std::atomic<bool> m_repeat{false};
    std::atomic<bool> m_stop_playing{false};
    std::thread m_playback_thread;
};

}
}
}

#endif
