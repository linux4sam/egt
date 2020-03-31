/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/detail/filesystem.h"
#include "egt/respath.h"
#include "egt/sound.h"
#include <alsa/asoundlib.h>
#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <deque>
#include <fstream>
#include <mutex>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>

#ifdef HAVE_SNDFILE
#include <sndfile.hh>
#endif

namespace egt
{
inline namespace v1
{

struct PlaybackThread
{
    PlaybackThread()
    {
        m_thread = std::thread(&PlaybackThread::run, this);
    }

    EGT_OPS_NOCOPY_NOMOVE(PlaybackThread);

    void run()
    {
        std::function<void()> task;
        while (true)
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            while (!m_stop && m_queue.empty())
                m_condition.wait(lock);

            if (m_stop)
                return;

            task = m_queue.front();
            m_queue.pop_front();
            m_condition.notify_one();
            lock.unlock();

            task();
        }
    }

    void enqueue(std::function<void()> job)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.emplace_back(std::move(job));
        m_condition.notify_one();
    }

    void quit()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_thread.joinable())
        {
            m_stop = true;
            m_condition.notify_all();
            lock.unlock();
            m_thread.join();
        }
    }

    ~PlaybackThread()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_thread.joinable())
        {
            m_stop = true;
            m_condition.notify_all();
            lock.unlock();
            m_thread.join();
        }
    }

    std::thread m_thread;
    std::deque<std::function<void()>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_stop{false};
};

namespace detail
{

struct soundimpl
{
    snd_pcm_t* handle{nullptr};
    snd_pcm_uframes_t frames{};
    int channels{0};
    unsigned int rate{0};
    PlaybackThread play_thread;
    std::atomic<bool> interrupt{false};
#ifdef HAVE_SNDFILE
    SndfileHandle in;
#else
    std::ifstream in;
#endif
};

}

namespace experimental
{

// NOLINTNEXTLINE(modernize-pass-by-value)
Sound::Sound(const std::string& uri, unsigned int rate, int channels, const std::string& device)
    : m_impl(std::make_unique<detail::soundimpl>()),
      m_uri(uri)
{
    open_alsa_device(device);
    init_alsa_params(rate, channels);
    open_file();
}

// NOLINTNEXTLINE(modernize-pass-by-value)
Sound::Sound(const std::string& uri, const std::string& device)
    : m_impl(std::make_unique<detail::soundimpl>()),
      m_uri(uri)
{
    open_alsa_device(device);
    open_file();
}

void Sound::open_file()
{
    std::string path;
    const auto type = detail::resolve_path(m_uri, path);

    if (type == detail::SchemeType::filesystem)
    {
        if (!detail::exists(path))
            throw std::runtime_error("file not found: " + path);
    }
    else
    {
        throw std::runtime_error("unsupported uri: " + m_uri);
    }

#ifdef HAVE_SNDFILE
    m_impl->in = SndfileHandle(path.c_str());
    init_alsa_params(m_impl->in.samplerate(), m_impl->in.channels());
#else
    if (m_impl->channels == 0 || m_impl->rate == 0)
    {
        spdlog::error("can't play sound file {}: sndfile not available and rate "
                      "and channel not specified", path);
        return;
    }

    m_impl->in.open(path, std::ios::binary | std::ios::in);
#endif

    if (!m_impl->in)
    {
        spdlog::error("can't open file: {}", path);
    }
}

void Sound::open_alsa_device(const std::string& device)
{
    static const auto MAX_RETRIES = 10;
    int tries = 0;
    int err;
    do
    {
        err = snd_pcm_open(&m_impl->handle, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
        if (err == -EBUSY)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } while (err == -EBUSY && ++tries <= MAX_RETRIES);

    if (err < 0)
    {
        throw std::runtime_error(fmt::format("can't open '{}' PCM device: {}",
                                             device, snd_strerror(err)));

    }
}

// default period size (number of frames per period)
static const auto PERIOD = 4096;

void Sound::init_alsa_params(unsigned int rate, int channels)
{
    snd_pcm_hw_params_t* params;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(m_impl->handle, params);

    auto err = snd_pcm_hw_params_set_access(m_impl->handle, params,
                                            SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0)
    {
        spdlog::error("can't set interleaved mode. {}", snd_strerror(err));
    }

    err = snd_pcm_hw_params_set_format(m_impl->handle, params,
                                       SND_PCM_FORMAT_S16_LE);
    if (err < 0)
    {
        spdlog::error("can't set format. {}", snd_strerror(err));
    }

    err = snd_pcm_hw_params_set_channels(m_impl->handle, params, channels);
    if (err < 0)
    {
        spdlog::error("can't set channels number. {}", snd_strerror(err));
    }

    err = snd_pcm_hw_params_set_rate_near(m_impl->handle, params, &rate, nullptr);
    if (err < 0)
    {
        spdlog::error("can't set rate. {}", snd_strerror(err));
    }

    /* write back hw parameters */
    err = snd_pcm_hw_params(m_impl->handle, params);
    if (err < 0)
    {
        spdlog::error("can't set harware parameters. {}", snd_strerror(err));
    }

    SPDLOG_TRACE("PCM name: {}", snd_pcm_name(m_impl->handle));
    SPDLOG_TRACE("PCM state: {}", snd_pcm_state_name(snd_pcm_state(m_impl->handle)));

    snd_pcm_hw_params_get_period_size(params, &m_impl->frames, nullptr);
    m_impl->channels = channels;
    m_impl->rate = rate;

    SPDLOG_TRACE("PCM frames: {}", m_impl->frames);
    SPDLOG_TRACE("PCM channels: {}", m_impl->channels);

    snd_pcm_sw_params_t* sw_params;

    err = snd_pcm_sw_params_malloc(&sw_params);
    if (err < 0)
    {
        spdlog::error("cannot allocate software parameters structure {}",
                      snd_strerror(err));
    }

    err = snd_pcm_sw_params_current(m_impl->handle, sw_params);
    if (err < 0)
    {
        spdlog::error("cannot initialize software parameters structure {}",
                      snd_strerror(err));
    }

    err = snd_pcm_sw_params_set_avail_min(m_impl->handle, sw_params, PERIOD);
    if (err < 0)
    {
        spdlog::error("cannot set minimum available count {}", snd_strerror(err));
    }

    err = snd_pcm_sw_params_set_start_threshold(m_impl->handle, sw_params, 0U);
    if (err < 0)
    {
        spdlog::error("cannot set start mode {}", snd_strerror(err));
    }

    err = snd_pcm_sw_params(m_impl->handle, sw_params);
    if (err < 0)
    {
        spdlog::error("cannot set software parameters {}",  snd_strerror(err));
    }

    snd_pcm_sw_params_free(sw_params);
}

void Sound::play(bool repeat)
{
    if (!m_impl->in)
        return;

    /*
     * So, this is not safe, sorta.  This is better left up to the playback
     * thread, but on some systems this seems to be the only way to *quickly*
     * abort the blocking calls in the playback thread.
     */
    snd_pcm_drop(m_impl->handle);

    // interrupt any pending playback
    m_impl->interrupt = true;

    // enqueue new playback
    m_impl->play_thread.enqueue([this, repeat]()
    {
        m_impl->interrupt = false;

        snd_pcm_prepare(m_impl->handle);

        std::vector<short> buf(PERIOD * m_impl->channels, 0);

        do
        {
#ifdef HAVE_SNDFILE
            m_impl->in.seek(0, SEEK_SET);
#else
            m_impl->in.clear();
            m_impl->in.seekg(0, std::ios::beg);
#endif

            while (true)
            {
                if (m_impl->interrupt)
                {
                    snd_pcm_drop(m_impl->handle);
                    return;
                }

                static const auto timeout_ms = 100;
                auto err = snd_pcm_wait(m_impl->handle, timeout_ms);
                if (err == 0)
                {
                    // timeout
                    continue;
                }
                else if (err < 0)
                {
                    // NOLINTNEXTLINE(misc-lambda-function-name)
                    SPDLOG_TRACE("PCM wait error");
                    return;
                }

                if (m_impl->interrupt)
                {
                    snd_pcm_drop(m_impl->handle);
                    return;
                }

                snd_pcm_sframes_t frames_to_deliver = snd_pcm_avail_update(m_impl->handle);
                if (frames_to_deliver < 0)
                {
                    if (frames_to_deliver == -EPIPE)
                    {
                        spdlog::error("PCM xrun occured\n");
                    }
                    else
                    {
                        spdlog::error("PCM unknown aail update return value: {}",
                                      frames_to_deliver);
                    }

                    break;
                }

                if (static_cast<snd_pcm_uframes_t>(frames_to_deliver) > m_impl->frames)
                {
                    frames_to_deliver = PERIOD;
                }

#ifdef HAVE_SNDFILE
                const auto count = m_impl->in.read(buf.data(), frames_to_deliver);
                if (count <= 0)
                {
                    break;
                }
#else
                m_impl->in.read(reinterpret_cast<char*>(buf.data()), frames_to_deliver * sizeof(buf[0]));
                const auto count = m_impl->in.gcount() / 2;
                if (!m_impl->in)
                {
                    break;
                }
#endif

                err = snd_pcm_writei(m_impl->handle, buf.data(), count / m_impl->channels);

                if (m_impl->interrupt)
                {
                    snd_pcm_drop(m_impl->handle);
                    return;
                }

                if (err == -EPIPE)
                    spdlog::error("underrun");

                if (err < 0)
                    err = snd_pcm_recover(m_impl->handle, err, 0);

                if (err > 0 && err < count / m_impl->channels)
                {
                    spdlog::error("short write to PCM device.");
                }
                else if (err < 0)
                {
                    spdlog::error("can't write to PCM device: {}", snd_strerror(err));
                    //break;
                }
            }
        } while (repeat);

        snd_pcm_drain(m_impl->handle);
    });
}

Sound::~Sound() noexcept
{
    m_impl->interrupt = true;
    m_impl->play_thread.quit();
    snd_pcm_close(m_impl->handle);
}

}
}
}
