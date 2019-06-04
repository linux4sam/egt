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
#include <egt/video.h>
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
void init_gst_thread()
{
    static bool init = false;
    if (!init)
    {
        gst_init(NULL, NULL);

        // cppcheck-suppress unreadVariable
        static std::thread t([]()
        {
            GMainLoop* mainloop = g_main_loop_new(NULL, FALSE);
            g_main_loop_run(mainloop);
        });
        init = true;
    }
}

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

} // End of detail.

VideoWindow::VideoWindow(const Size& size, pixel_format format, windowhint hint)
    : Window(size, flags_type(), (detail::is_target_sama5d4() ? pixel_format::xrgb8888 : format), hint),
      m_loopback(false)
{
    set_boxtype(Theme::boxtype::none);
    createImpl(size);
}

VideoWindow::VideoWindow(const Rect& rect, pixel_format format, windowhint hint)
    : Window(rect, flags_type(), (detail::is_target_sama5d4() ? pixel_format::xrgb8888 : format), hint),
      m_loopback(false)
{
    set_boxtype(Theme::boxtype::none);
    createImpl(rect.size());
}

void VideoWindow::createImpl(const Size& size)
{
    detail::init_gst_thread();

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
    return m_decoderImpl->get_position();
}

int64_t VideoWindow::duration() const
{
    return m_decoderImpl->get_duration();
}

bool VideoWindow::set_media(const string& uri)
{
    return m_decoderImpl->set_media(uri);
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

double VideoWindow::get_volume() const
{
    return m_decoderImpl->get_volume();
}

bool VideoWindow::seek(const int64_t time)
{
    return m_decoderImpl->seek(time);
}

std::string VideoWindow::get_error_message() const
{
    return m_decoderImpl->get_error_message();
}

VideoWindow::~VideoWindow()
{
    m_decoderImpl->destroyPipeline();
}

} //namespace v1

} //namespace egt
