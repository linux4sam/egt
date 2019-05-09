/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/app.h"
#include <egt/detail/video/gstappsinkimpl.h>
#include <egt/detail/video/gstkmssinkimpl.h>
#include <egt/video.h>
#include <string>
#include <thread>
#include <fstream>

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

bool is_target_sama5d4()
{
    std::ifstream infile("/proc/device-tree/model");
    if (infile.is_open())
    {
        std::string line;
        while (getline(infile, line))
        {
            DBG("CPU: " << line);
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
    : Window(size, flags_type(), (detail::is_target_sama5d4() ? pixel_format::xrgb8888 : format), hint)
{
    set_boxtype(Theme::boxtype::none);
    createImpl(size);
}

VideoWindow::VideoWindow(const Rect& rect, pixel_format format, windowhint hint)
    : Window(rect, flags_type(), (detail::is_target_sama5d4() ? pixel_format::xrgb8888 : format), hint)
{
    set_boxtype(Theme::boxtype::none);
    createImpl(rect.size());
}

void VideoWindow::createImpl(const Size& size)
{
    detail::init_gst_thread();

#ifdef HAVE_LIBPLANES
    DBG("VideoWindow: HAVE_LIBPLANES");
    if (flags().is_set(Widget::flag::plane_window) && detail::is_target_sama5d4())
    {
        DBG("VideoWindow: Using KMS sink");
        m_decoderImpl.reset(new detail::GstKmsSinkImpl(*this, size, detail::is_target_sama5d4()));
    }
    else
#endif
    {
        DBG("VideoWindow: Using APP sink");
        m_decoderImpl.reset(new detail::GstAppSinkImpl(*this, size));
    }
}

void VideoWindow::draw(Painter& painter, const Rect& rect)
{
    m_decoderImpl->draw(painter, rect);
}

uint64_t VideoWindow::position()
{
    return m_decoderImpl->get_position();
}

uint64_t VideoWindow::duration()
{
    return m_decoderImpl->get_duration();
}

void VideoWindow::set_media_info(const std::string& info, const int type)
{
    DBG("VideoWindow: In " << __func__ << " Info = " << info);
    if (type == 0)
    {
        m_ctype = info;
    }
    else if (type == 1)
    {
        m_vtrack = true;
        m_vcodec = info;
    }
    else if (type == 2)
    {
        m_atrack = true;
        m_acodec = info;
    }
}

bool VideoWindow::set_media(const string& uri)
{

    m_filename = uri;

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
    invoke_handlers(eventid::property_changed);
    return true;
}

double VideoWindow::get_volume()
{
    return m_decoderImpl->get_volume();
}

bool VideoWindow::seek(int64_t time_nanoseconds)
{
    return m_decoderImpl->seek_to_time(time_nanoseconds);
}

void VideoWindow::handle_gst_events(detail::gsteventid event)
{
    switch (event)
    {
    case detail::gsteventid::gst_progress:
        invoke_handlers(eventid::property_changed);
        break;
    case detail::gsteventid::gst_stop:
        m_decoderImpl->destroyPipeline();
        invoke_handlers(eventid::event1);
        break;
    case detail::gsteventid::gst_error:
        invoke_handlers(eventid::event2);
        break;
    default:
        break;
    }
}

VideoWindow::~VideoWindow()
{
    m_decoderImpl->destroyPipeline();
}

} //namespace v1

} //namespace egt
