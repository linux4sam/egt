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
            spdlog::info("Sound : {}", line);
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

VideoWindow::VideoWindow(const Rect& rect, pixel_format format, windowhint hint)
    : Window(rect, (detail::is_target_sama5d4() ? pixel_format::xrgb8888 : format), hint),
      m_loopback(false)
{
    set_boxtype(Theme::boxtype::none);

    createImpl(rect.size());
}

VideoWindow::VideoWindow(const Rect& rect, const std::string& uri,
                         pixel_format format, windowhint hint)
    : VideoWindow(rect, format, hint)
{
    set_media(uri);
}

void VideoWindow::createImpl(const Size& size)
{
    if (flags().is_set(Widget::flag::plane_window) && detail::is_target_sama5d4())
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

bool VideoWindow::set_media(const string& uri)
{
    return m_decoderImpl->set_media(detail::abspath(uri));
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

bool VideoWindow::set_volume(double volume)
{
    m_decoderImpl->set_volume(volume);
    return true;
}

double VideoWindow::volume() const
{
    return m_decoderImpl->volume();
}

bool VideoWindow::seek(int64_t time)
{
    return m_decoderImpl->seek(time);
}

void VideoWindow::set_scale(float scalex, float scaley)
{
    auto xs = detail::change_if_diff<float>(m_scalex, scalex);
    auto ys = detail::change_if_diff<float>(m_scaley, scaley);

    if (xs || ys)
    {
        if (!flags().is_set(Widget::flag::plane_window))
        {
            m_decoderImpl->scale(m_scalex, m_scaley);
        }
        else
        {
            Window::set_scale(m_scalex, m_scaley);
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
