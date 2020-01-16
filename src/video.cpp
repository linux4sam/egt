/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/video/gstappsinkimpl.h"
#include "detail/video/gstkmssinkimpl.h"
#include "egt/app.h"
#include "egt/detail/filesystem.h"
#include "egt/video.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <string>
#include <thread>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Check if target device is sama5d4 for using hardware
 * decoder.
 */

bool is_target_sama5d4()
{
    std::ifstream infile("/proc/device-tree/model");
    if (infile.is_open())
    {
        std::string line;
        while (getline(infile, line))
        {
            SPDLOG_DEBUG("CPU: {}", line);
            if (line.find("SAMA5D4") != std::string::npos)
            {
                infile.close();
                return true;
            }
        }
        infile.close();
    }
    return false;
}

bool audio_device()
{
    std::ifstream infile("/proc/asound/cards");
    if (infile.is_open())
    {
        std::string line;
        while (getline(infile, line))
        {
            SPDLOG_DEBUG("Sound : {}", line);
            if (line.find("no soundcards") != std::string::npos)
            {
                infile.close();
                return false;
            }
        }
        infile.close();
        return true;
    }
    return false;
}

} // End of detail.

VideoWindow::VideoWindow(const Rect& rect, PixelFormat format, WindowHint hint)
    : Window(rect, (detail::is_target_sama5d4() ? PixelFormat::xrgb8888 : format), hint),
      m_loopback(false)
{
    boxtype().clear();

    createImpl(rect.size());
}

VideoWindow::VideoWindow(const Rect& rect, const std::string& uri,
                         PixelFormat format, WindowHint hint)
    : VideoWindow(rect, format, hint)
{

    if (!media(uri))
    {
        std::ostringstream ss;
        ss << "Error: failed to initalize gstreamer pipeline";
        throw std::runtime_error(ss.str());
    }
}

void VideoWindow::createImpl(const Size& size)
{
    if (flags().is_set(Widget::Flag::plane_window) && detail::is_target_sama5d4())
    {
        SPDLOG_DEBUG("VideoWindow: Using KMS sink");
#ifdef HAVE_LIBPLANES
        m_decoderImpl.reset(new detail::GstKmsSinkImpl(*this, size, detail::is_target_sama5d4()));
#endif
    }
    else
    {
        SPDLOG_DEBUG("VideoWindow: Using APP sink");
        m_decoderImpl.reset(new detail::GstAppSinkImpl(*this, size));
    }
}

void VideoWindow::draw(Painter& painter, const Rect& rect)
{
    m_decoderImpl->draw(painter, rect);
}

int64_t VideoWindow::position() const
{
    return m_decoderImpl->position();
}

int64_t VideoWindow::duration() const
{
    return m_decoderImpl->duration();
}

bool VideoWindow::media(const string& uri)
{
    return m_decoderImpl->media(detail::abspath(uri));
}

bool VideoWindow::pause()
{
    return m_decoderImpl->pause();
}

bool VideoWindow::playing() const
{
    return m_decoderImpl->playing();
}

bool VideoWindow::play()
{
    return m_decoderImpl->play();
}

bool VideoWindow::volume(int volume)
{
    m_decoderImpl->volume(volume);
    return true;
}

int VideoWindow::volume() const
{
    return m_decoderImpl->volume();
}

bool VideoWindow::seek(int64_t pos)
{
    return m_decoderImpl->seek(pos);
}

void VideoWindow::scale(float hscale, float vscale)
{
    auto xs = detail::change_if_diff<float>(m_scalex, hscale);
    auto ys = detail::change_if_diff<float>(m_scaley, vscale);

    if (xs || ys)
    {
        if (!flags().is_set(Widget::Flag::plane_window))
        {
            m_decoderImpl->scale(m_scalex, m_scaley);
        }
        else
        {
            Window::scale(m_scalex, m_scaley);
        }
    }
}

bool VideoWindow::has_audio() const
{
    return m_decoderImpl->has_audio();
}

std::string VideoWindow::error_message() const
{
    return m_decoderImpl->error_message();
}

VideoWindow::~VideoWindow()
{
    m_decoderImpl->destroyPipeline();
}

} //namespace v1

} //namespace egt
