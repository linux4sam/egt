/*
 * Copyright (C) 2025 Microchip Technology Inc. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sound_effect_impl.h"

#include <mutex>
#include <sstream>
#include <stdexcept>

#include <alsa/asoundlib.h>
#include <sndfile.hh>

#include "detail/egtlog.h"
#include "egt/detail/filesystem.h"
#include "egt/respath.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

std::once_flag SoundEffectImpl::init_flag;
int SoundEffectImpl::ipc_key = 7557;

SoundEffectImpl::SoundEffectImpl()
{
    /*
     * Configure ALSA, when a sound effect is created for the first time.
     * This process involves setting up the ALSA configuration to create the
     * PCM devices for EGT.
     */
    std::call_once(init_flag, []()
    {
        if (!init())
        {
            const std::string error = "Can't setup EGT PCM devices";
            EGTLOG_ERROR(error);
            throw std::system_error(std::make_error_code(std::errc::no_such_device), error);
        }
    });
}

SoundEffectImpl::SoundEffectImpl(const std::string& uri, const SoundEffect::SoundDevice& sound_device)
    : SoundEffectImpl()
{
    device(sound_device);

    if (!media(uri))
    {
        const std::string error = "Can't open or load the media file";
        EGTLOG_ERROR(error);
        throw std::runtime_error(error);
    }
}

SoundEffectImpl::~SoundEffectImpl() noexcept
{
    if (m_snd_pcm_handle)
        snd_pcm_close(m_snd_pcm_handle);
}

bool SoundEffectImpl::init()
{
    const auto sound_devices = sound_device_list();

    if (sound_devices.empty())
        return false;

    for (const auto& sound_device : sound_devices)
    {
        EGTLOG_DEBUG("Create plugin");
        if (!create_plugins(sound_device.card_index(), sound_device.device_index()))
            return false;

        // It causes a lot of logs, so uncomment it, only for debug purpose.
        //dump_alsa_conf(snd_config, 0);
    }

    return true;
}

bool SoundEffectImpl::create_plugins(int card, int dev)
{
    /*
     * Create a dmix plugin on top of the hardware device to manage the mixing
     * of multiple streams. Additionally, Create a plug plugin to handle
     * channel, rate, and format conversion.
     */
    std::ostringstream oss;
    oss << "pcm.egt_" << card << "_" << dev << " {\n"
        << "    type plug\n"
        << "    slave {\n"
        << "        pcm {\n"
        << "            type dmix\n"
        << "            ipc_key " << ipc_key++ << "\n"
        << "            slave {\n"
        << "                pcm \"hw:" << card << "," << dev << "\"\n"
        << "            }\n"
        << "        }\n"
        << "    }\n"
        << "}\n";
    const auto egt_config = oss.str();
    EGTLOG_TRACE("{}", egt_config);

    snd_config_t* egt_devices;
    auto err = snd_config_load_string(&egt_devices, egt_config.c_str(), egt_config.length());
    if (err)
    {
        EGTLOG_ERROR("{}", snd_strerror(err));
        return false;
    }

    err = snd_config_update();
    if (err < 0)
    {
        EGTLOG_ERROR("{}", snd_strerror(err));
        snd_config_delete(egt_devices);
        return false;
    }

    if ((err = snd_config_merge(snd_config, egt_devices, 0)))
    {
        EGTLOG_ERROR("{}", snd_strerror(err));
        snd_config_delete(egt_devices);
        return false;
    }

    return true;
}

bool SoundEffectImpl::open_pcm_device()
{
    static constexpr int MAX_RETRIES = 5;
    int tries = 0;
    int err;

    std::ostringstream oss;
    oss << "egt_" << m_sound_device.card_index() << "_" << m_sound_device.device_index();
    const auto pcm = oss.str();

    do
    {
        EGTLOG_DEBUG("Trying to open PCM device: {} (Attempt {}/{})", pcm, tries + 1, MAX_RETRIES);
        err = snd_pcm_open(&m_snd_pcm_handle, pcm.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
        if (err == -EBUSY)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } while (err == -EBUSY && ++tries < MAX_RETRIES);

    if (err < 0)
    {
        EGTLOG_ERROR("Failed to open PCM device {}: {}", pcm, snd_strerror(err));
        return false;
    }

    return true;
}

bool SoundEffectImpl::open_file(const std::string& path)
{
    /*
     * If there is a sound effect that is playing, stop it, as sndfile and the
     * pcm device will be configured according to the file to play.
     */
    stop();

    m_sndfile_handle = std::make_unique<SndfileHandle>(path.c_str());
    if (!m_sndfile_handle)
    {
        EGTLOG_ERROR("Can't get sndfile for {}", path);
        return false;
    }

    m_channels = m_sndfile_handle->channels();
    if (!m_channels)
    {
        EGTLOG_ERROR("No channel found for {}", path);
        return false;
    }

    m_format = m_sndfile_handle->format();
    m_samplerate = m_sndfile_handle->samplerate();

    EGTLOG_DEBUG("File: path: {}", path);
    EGTLOG_DEBUG("File: format: {}, {}, {}", string_of_major_format(m_format),
                 string_of_minor_format(m_format), string_of_endianness_format(m_format));
    EGTLOG_DEBUG("File: frames: {}, channels: {}, samplerate: {}",
                 m_sndfile_handle->frames(), m_channels, m_samplerate);

    // Configure the pcm device according to the audio file parameters.
    return configure_pcm_device();
}

bool SoundEffectImpl::configure_pcm_device()
{
    snd_pcm_hw_params_t* hw_params;
    snd_pcm_hw_params_alloca(&hw_params);

    auto err = snd_pcm_hw_params_any(m_snd_pcm_handle, hw_params);
    if (err < 0)
    {
        EGTLOG_ERROR(fmt::format("Can't get hw parameters: {}", snd_strerror(err)));
        return false;
    }

    err = snd_pcm_hw_params_set_rate_resample(m_snd_pcm_handle, hw_params, 1); //TODO
    if (err < 0)
    {
        EGTLOG_ERROR(fmt::format("Resampling setup failed for playback: {}", snd_strerror(err)));
        return false;
    }

    err = snd_pcm_hw_params_set_access(m_snd_pcm_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0)
    {
        EGTLOG_ERROR(fmt::format("Access type not available for playback: {}", snd_strerror(err)));
        return false;
    }

    m_pcm_format = to_pcm_format(m_format);
    err = snd_pcm_hw_params_set_format(m_snd_pcm_handle, hw_params, m_pcm_format);
    if (err < 0)
    {
        EGTLOG_ERROR(fmt::format("Sample format ({}) not available for playback: {}", m_pcm_format, snd_strerror(err)));
        return false;
    }

    err = snd_pcm_hw_params_set_channels(m_snd_pcm_handle, hw_params, m_channels);
    if (err < 0)
    {
        EGTLOG_ERROR(fmt::format("Channels count ({}) not available for playback: {}", m_channels, snd_strerror(err)));
        return false;
    }

    const auto samplerate_requested = m_samplerate;
    err = snd_pcm_hw_params_set_rate_near(m_snd_pcm_handle, hw_params, &m_samplerate, nullptr);
    if (err < 0)
    {
        EGTLOG_ERROR(fmt::format("Rate {} Hz not available for playback: {}", m_samplerate, snd_strerror(err)));
        return false;
    }

    if (samplerate_requested != m_samplerate)
    {
        EGTLOG_ERROR(fmt::format("Rate doesn't match (requested {}Hz, get {}Hz)", samplerate_requested, m_samplerate));
        return false;
    }

    err = snd_pcm_hw_params(m_snd_pcm_handle, hw_params);
    if (err < 0)
    {
        EGTLOG_ERROR(fmt::format("Unable to set hw params for playback: {}", snd_strerror(err)));
        return false;
    }

    EGTLOG_DEBUG("PCM name: {}", snd_pcm_name(m_snd_pcm_handle));
    EGTLOG_DEBUG("PCM state: {}", snd_pcm_name(m_snd_pcm_handle));

    err = snd_pcm_hw_params_get_period_size(hw_params, &m_period_size, nullptr);
    if (err < 0)
    {
        EGTLOG_ERROR(fmt::format("Unable to get period size for playback: {}", snd_strerror(err)));
        return false;
    }

    snd_pcm_uframes_t buffer_size;
    snd_pcm_hw_params_get_buffer_size(hw_params, &buffer_size);
    if (err < 0)
        EGTLOG_WARN(fmt::format("Unable to get buffer size for playback: {}", snd_strerror(err)));

    EGTLOG_DEBUG("PCM frame size: {} bytes", snd_pcm_frames_to_bytes(m_snd_pcm_handle, 1));
    EGTLOG_DEBUG("PCM period size: {} frames", m_period_size);
    EGTLOG_DEBUG("PCM buffer size: {} frames", buffer_size);

    snd_pcm_sw_params_t* sw_params;
    snd_pcm_sw_params_alloca(&sw_params);

    err = snd_pcm_sw_params_current(m_snd_pcm_handle, sw_params);
    if (err < 0)
        EGTLOG_WARN(fmt::format("Unable to determine current swparams for playback: {}", snd_strerror(err)));

    err = snd_pcm_sw_params_set_start_threshold(m_snd_pcm_handle, sw_params, m_period_size);
    if (err < 0)
        EGTLOG_WARN(fmt::format("Unable to set start threshold for playback: {}", snd_strerror(err)));

    err = snd_pcm_sw_params_set_stop_threshold(m_snd_pcm_handle, sw_params, buffer_size);
    if (err < 0)
        EGTLOG_WARN(fmt::format("Unable to set stop threshold for playback: {}", snd_strerror(err)));

    err = snd_pcm_sw_params_set_avail_min(m_snd_pcm_handle, sw_params, m_period_size);
    if (err < 0)
        EGTLOG_WARN(fmt::format("Unable to set available min for playback: {}", snd_strerror(err)));

    err = snd_pcm_sw_params(m_snd_pcm_handle, sw_params);
    if (err < 0)
        EGTLOG_WARN(fmt::format("Unable to set sw params for playback: {}", snd_strerror(err)));

    return true;
}

bool SoundEffectImpl::device(const SoundEffect::SoundDevice& sound_device) noexcept
{
    auto const card = sound_device.card_index();
    auto const dev = sound_device.device_index();
    EGTLOG_DEBUG("Set device: {},{}", card, dev);

    stop();

    if (m_snd_pcm_handle)
    {
        snd_pcm_close(m_snd_pcm_handle);
        m_snd_pcm_handle = nullptr;
    }

    snd_ctl_card_info_t* info;
    snd_ctl_card_info_alloca(&info);

    snd_pcm_info_t* pcminfo;
    snd_pcm_info_alloca(&pcminfo);


    int err;
    snd_ctl_t* handle;
    const std::string name = "hw:" + std::to_string(card);
    if ((err = snd_ctl_open(&handle, name.c_str(), 0)) < 0)
    {
        EGTLOG_ERROR("Control open ({}): {}", card, snd_strerror(err));
        return false;
    }

    if ((err = snd_ctl_card_info(handle, info)) < 0)
    {
        EGTLOG_ERROR("Control hardware info ({}): {}", card, snd_strerror(err));
        snd_ctl_close(handle);
        return false;
    }

    snd_pcm_info_set_device(pcminfo, dev);
    snd_pcm_info_set_subdevice(pcminfo, 0);
    snd_pcm_info_set_stream(pcminfo, SND_PCM_STREAM_PLAYBACK);
    if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0)
    {
        EGTLOG_ERROR("Control digital audio info ({}): {}", card, snd_strerror(err));
        return false;
    }

    m_sound_device = sound_device;
    m_sound_device.card_name(snd_ctl_card_info_get_name(info));
    m_sound_device.device_name(snd_pcm_info_get_name(pcminfo));

    if (!open_pcm_device())
        return false;

    // Don't forget to configure the pcm device according to the audio file parameters.
    if (!m_uri.empty())
        return configure_pcm_device();

    return true;
}

bool SoundEffectImpl::media(const std::string& uri) noexcept
{
    std::string path;
    const auto type = detail::resolve_path(uri, path);
    if (type == detail::SchemeType::filesystem)
    {
        if (!detail::exists(path))
        {
            EGTLOG_ERROR("{} doesn't exist", path);
            return false;
        }
    }
    else
    {
        EGTLOG_ERROR("Unsupported uri: {}", uri);
        return false;
    }

    // open_file calls configure_pcm_device automatically.
    if (!open_file(path))
        return false;

    m_uri = uri;

    return true;
}

void SoundEffectImpl::play()
{
    EGTLOG_DEBUG("Playing sound effect");

    // If the sound is playing, stop it
    if (m_playback_thread.joinable())
    {
        m_stop_playing = true;
        m_playback_thread.join();
    }

    m_stop_playing = false;
    m_playback_thread = std::thread(&SoundEffectImpl::playback, this);
}

void SoundEffectImpl::stop()
{
    EGTLOG_DEBUG("Stopping sound effect");

    m_stop_playing = true;
    if (m_playback_thread.joinable())
        m_playback_thread.join();
}

void SoundEffectImpl::playback()
{
    snd_pcm_prepare(m_snd_pcm_handle);

    const auto buffer_size = snd_pcm_frames_to_bytes(m_snd_pcm_handle, m_period_size);
    std::unique_ptr<void, decltype(&std::free)> buffer(malloc(buffer_size), &std::free);
    EGTLOG_DEBUG("playback buffer size: {} bytes", buffer_size);

    do
    {
        m_sndfile_handle->seek(0, SEEK_SET);

        while (true)
        {
            if (m_stop_playing)
            {
                EGTLOG_DEBUG("stop playback as requested");
                return;
            }

            static const auto TIMEOUT_MS = 10;
            auto err = snd_pcm_wait(m_snd_pcm_handle, TIMEOUT_MS);
            if (err == 0)
            {
                EGTLOG_TRACE("PCM wait timeout");
                continue;
            }
            else if (err < 0)
            {
                EGTLOG_DEBUG("PCM wait error {}", snd_strerror(err));
                return;
            }

            snd_pcm_sframes_t frames_to_deliver = snd_pcm_avail_update(m_snd_pcm_handle);
            EGTLOG_TRACE("frames_to_deliver: {}", frames_to_deliver);
            if (frames_to_deliver < 0)
            {
                if (frames_to_deliver == -EPIPE)
                    detail::error("PCM xrun occurred\n");
                else
                    detail::error("PCM unknown avail update return value: {}",
                                  frames_to_deliver);
                return;
            }

            if (static_cast<snd_pcm_uframes_t>(frames_to_deliver) > m_period_size)
                frames_to_deliver = m_period_size;
            EGTLOG_TRACE("frames_to_deliver update: {}", frames_to_deliver);

            sf_count_t count;
            if (m_pcm_format == SND_PCM_FORMAT_S32)
                count = m_sndfile_handle->readf(static_cast<int*>(buffer.get()), frames_to_deliver);
            else
                count = m_sndfile_handle->readf(static_cast<short*>(buffer.get()), frames_to_deliver);
            EGTLOG_TRACE("sndfile read count: {}", count);

            if (count <= 0)
                break;

            err = snd_pcm_writei(m_snd_pcm_handle, buffer.get(), count);
            if (err < 0)
                err = snd_pcm_recover(m_snd_pcm_handle, err, 0);

            if (err > 0 && err < count)
            {
                EGTLOG_WARN("PCM short write");
            }
            else if (err < 0)
            {
                EGTLOG_ERROR("PCM cannot recover from error: {}", snd_strerror(err));
                return;
            }
        }
        EGTLOG_DEBUG("repeat? {}", m_repeat ? "yes" : "no");
    } while (m_repeat);
}

std::vector<SoundEffect::SoundDevice> SoundEffectImpl::sound_device_list(void)
{
    int card = -1;
    if (snd_card_next(&card) < 0 || card < 0)
    {
        EGTLOG_ERROR("No soundcards found...");
        return {};
    }

    EGTLOG_DEBUG("List of hardware devices:");

    std::vector<SoundEffect::SoundDevice> sound_devices = {};

    snd_ctl_card_info_t* info;
    snd_ctl_card_info_alloca(&info);

    snd_pcm_info_t* pcminfo;
    snd_pcm_info_alloca(&pcminfo);

    while (card >= 0)
    {
        int dev = -1;

        const std::string name = "hw:" + std::to_string(card);
        EGTLOG_DEBUG("{}", name);

        snd_ctl_t* handle;
        int err;
        if ((err = snd_ctl_open(&handle, name.c_str(), 0)) < 0)
        {
            EGTLOG_ERROR("Control open ({}): {}", card, snd_strerror(err));
            goto next_card;
        }

        if ((err = snd_ctl_card_info(handle, info)) < 0)
        {
            EGTLOG_ERROR("Control hardware info ({}): {}", card, snd_strerror(err));
            snd_ctl_close(handle);
            goto next_card;
        }

        while (true)
        {
            if (snd_ctl_pcm_next_device(handle, &dev) < 0)
                EGTLOG_ERROR("Snd_ctl_pcm_next_device");
            if (dev < 0)
                break;

            snd_pcm_info_set_device(pcminfo, dev);
            snd_pcm_info_set_subdevice(pcminfo, 0);
            snd_pcm_info_set_stream(pcminfo, SND_PCM_STREAM_PLAYBACK);

            if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0)
            {
                if (err != -ENOENT)
                    EGTLOG_ERROR("Control digital audio info ({}): {}", card, snd_strerror(err));
                continue;
            }

            SoundEffect::SoundDevice sound_device(card, dev);
            sound_device.card_name(snd_ctl_card_info_get_name(info));
            sound_device.device_name(snd_pcm_info_get_name(pcminfo));

            EGTLOG_DEBUG("card {}: {}, device {}: {}",
                         card, sound_device.card_name(),
                         dev, sound_device.device_name());

            sound_devices.push_back(std::move(sound_device));
        }
        snd_ctl_close(handle);

next_card:
        if ((err = snd_card_next(&card)) < 0)
        {
            EGTLOG_ERROR("Sound card next: {}", snd_strerror(err));
            break;
        }
    }

    return sound_devices;
}

constexpr snd_pcm_format_t SoundEffectImpl::to_pcm_format(int format)
{
    switch (format & SF_FORMAT_SUBMASK)
    {
    case SF_FORMAT_PCM_U8: return SND_PCM_FORMAT_U8;
    case SF_FORMAT_PCM_16: return SND_PCM_FORMAT_S16;
    case SF_FORMAT_PCM_24: return SND_PCM_FORMAT_S16;
    case SF_FORMAT_PCM_32: return SND_PCM_FORMAT_S32;
    default: return SND_PCM_FORMAT_S16;
    };
}

std::string SoundEffectImpl::string_of_major_format(int format)
{
    switch (format & SF_FORMAT_TYPEMASK)
    {
    case SF_FORMAT_WAV: return "wav";
    case SF_FORMAT_AIFF: return "aiff";
    case SF_FORMAT_AU: return "au";
    case SF_FORMAT_RAW: return "raw";
    case SF_FORMAT_PAF: return "paf";
    case SF_FORMAT_SVX: return "svx";
    case SF_FORMAT_NIST: return "nist";
    case SF_FORMAT_VOC: return "voc";
    case SF_FORMAT_IRCAM: return "ircam";
    case SF_FORMAT_W64: return "w64";
    case SF_FORMAT_MAT4: return "mat4";
    case SF_FORMAT_MAT5: return "mat5";
    case SF_FORMAT_PVF: return "pvf";
    case SF_FORMAT_XI: return "xi";
    case SF_FORMAT_HTK: return "htk";
    case SF_FORMAT_SDS: return "sds";
    case SF_FORMAT_AVR: return "avr";
    case SF_FORMAT_WAVEX: return "wavex";
    case SF_FORMAT_SD2: return "sd2";
    case SF_FORMAT_FLAC: return "flac";
    case SF_FORMAT_CAF: return "caf";
    case SF_FORMAT_WVE: return "wfe";
    case SF_FORMAT_OGG: return "ogg";
    case SF_FORMAT_MPC2K: return "mpc2k";
    case SF_FORMAT_RF64: return "rf64";
    case SF_FORMAT_MPEG: return "mpeg";
    default: return "unknown";
    };
}

std::string SoundEffectImpl::string_of_minor_format(int format)
{
    switch (format & SF_FORMAT_SUBMASK)
    {
    case SF_FORMAT_PCM_S8: return "int8";
    case SF_FORMAT_PCM_16: return "int16";
    case SF_FORMAT_PCM_24: return "int24";
    case SF_FORMAT_PCM_32: return "int32";
    case SF_FORMAT_PCM_U8: return "uint8";
    case SF_FORMAT_FLOAT: return "float";
    case SF_FORMAT_DOUBLE: return "double";
    case SF_FORMAT_ULAW: return "ulaw";
    case SF_FORMAT_ALAW: return "alaw";
    case SF_FORMAT_IMA_ADPCM: return "ima_adpcm";
    case SF_FORMAT_MS_ADPCM: return "ms_adpcm";
    case SF_FORMAT_GSM610: return "gsm610";
    case SF_FORMAT_VOX_ADPCM: return "vox_adpcm";
    case SF_FORMAT_NMS_ADPCM_16: return "nms_adpcm_16";
    case SF_FORMAT_NMS_ADPCM_24: return "nms_adpcm_24";
    case SF_FORMAT_NMS_ADPCM_32: return "nms_adpcm_32";
    case SF_FORMAT_G721_32: return "g721_32";
    case SF_FORMAT_G723_24: return "g723_24";
    case SF_FORMAT_G723_40: return "g723_40";
    case SF_FORMAT_DWVW_12: return "dwvw_12";
    case SF_FORMAT_DWVW_16: return "dwvw_16";
    case SF_FORMAT_DWVW_24: return "dwvw_24";
    case SF_FORMAT_DWVW_N: return "dwvw_n";
    case SF_FORMAT_DPCM_8: return "dpcm_8";
    case SF_FORMAT_DPCM_16: return "dpcm_16";
    case SF_FORMAT_VORBIS: return "vorbis";
    case SF_FORMAT_OPUS: return "opus";
    case SF_FORMAT_ALAC_16: return "alac_16";
    case SF_FORMAT_ALAC_20: return "alac_20";
    case SF_FORMAT_ALAC_24: return "alac_24";
    case SF_FORMAT_ALAC_32: return "alac_32";
    case SF_FORMAT_MPEG_LAYER_I: return "mpeg_layer_I";
    case SF_FORMAT_MPEG_LAYER_II: return "mpeg_layer_II";
    case SF_FORMAT_MPEG_LAYER_III: return "mpeg_layer_III";
    default: return "unknown";
    };
}

std::string SoundEffectImpl::string_of_endianness_format(int format)
{
    switch (format & SF_FORMAT_ENDMASK)
    {
    case SF_ENDIAN_FILE: return "file_endianness";
    case SF_ENDIAN_LITTLE: return "little_endianness";
    case SF_ENDIAN_BIG: return "big_endianness";
    case SF_ENDIAN_CPU: return "cpu_endianness";
    default: return "unknown";
    };
}

// Utility method for debug purpose.
void SoundEffectImpl::dump_alsa_conf(snd_config_t* config, int offset)
{
    const char* id;
    auto err = snd_config_get_id(config, &id);
    if (err)
        EGTLOG_ERROR("{}", snd_strerror(err));

    std::string str_offset = {};
    for (int i = 0; i < offset; ++i)
        str_offset.append("    ");

    snd_config_type_t type = snd_config_get_type(config);
    if (type == SND_CONFIG_TYPE_COMPOUND)
    {
        EGTLOG_DEBUG("{}{} {}", str_offset, (id == nullptr) ? "" : id, "{");

        // If it's a compound node, iterate over its children
        snd_config_iterator_t pos, next;
        snd_config_for_each(pos, next, config)
        {
            snd_config_t* current_child_config;
            current_child_config = snd_config_iterator_entry(pos);
            dump_alsa_conf(current_child_config, offset + 1);
        }
        EGTLOG_DEBUG("{}{}", str_offset, "}");
    }
    else if (type == SND_CONFIG_TYPE_STRING)
    {
        const char* value;
        err = snd_config_get_string(config, &value);
        if (err)
            EGTLOG_ERROR("{}", snd_strerror(err));

        EGTLOG_DEBUG("{}{} {}", str_offset, id, value);
    }
    else if (type == SND_CONFIG_TYPE_INTEGER)
    {
        long value;
        err = snd_config_get_integer(config, &value);
        if (err)
            EGTLOG_ERROR("{}", snd_strerror(err));

        EGTLOG_DEBUG("{}{} {}", str_offset, id, value);
    }
    else if (type == SND_CONFIG_TYPE_INTEGER64)
    {
        long long value;
        err = snd_config_get_integer64(config, &value);
        if (err)
            EGTLOG_ERROR("{}", snd_strerror(err));

        EGTLOG_DEBUG("{}{} {}", str_offset, id, value);
    }
    else if (type == SND_CONFIG_TYPE_REAL)
    {
        double value;
        err = snd_config_get_real(config, &value);
        if (err)
            EGTLOG_ERROR("{}", snd_strerror(err));

        EGTLOG_DEBUG("{}{} {}", str_offset, id, value);
    }
    else
    {
        EGTLOG_WARN("type not handled");
    }
}

}
}
}
