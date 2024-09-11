/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/egtlog.h"
#include "detail/multimedia/gstdecoderimpl.h"
#include "egt/app.h"
#include "egt/detail/filesystem.h"
#include "egt/respath.h"
#include "egt/video.h"
#include <fstream>
#include <string>
#include <thread>

namespace egt
{
inline namespace v1
{
namespace detail
{

/*
 * Check if audio playback device is present
 */
bool audio_device()
{
    std::ifstream infile("/proc/asound/devices");
    if (infile.is_open())
    {
        std::string line;
        while (getline(infile, line))
        {
            EGTLOG_DEBUG("Sound : {}", line);
            if (line.find("audio playback") != std::string::npos)
            {
                infile.close();
                return true;
            }
        }
        infile.close();
    }
    return false;
}

WindowHint check_windowhint(WindowHint& hint)
{
    if (hint == WindowHint::cursor_overlay)
    {
        throw std::runtime_error("Cannot create Videowindow with cursor_overlay hint");
    }
    return hint;
}

} // End of detail.

VideoWindow::VideoWindow(const Rect& rect, PixelFormat format, WindowHint hint)
    : Window(rect, format, detail::check_windowhint(hint)),
      m_video_impl(std::make_unique<detail::GstDecoderImpl>(this, rect.size())),
      on_position_changed(&m_video_impl->on_position_changed),
      on_error(&m_video_impl->on_error),
      on_eos(&m_video_impl->on_eos),
      on_state_changed(&m_video_impl->on_state_changed)
{
    fill_flags().clear();
}

VideoWindow::VideoWindow(const Rect& rect, const std::string& uri,
                         PixelFormat format, WindowHint hint)
    : VideoWindow(rect, format, hint)
{
    if (!media(uri))
    {
        throw std::runtime_error("failed to initialize gstreamer pipeline");
    }
}

VideoWindow::VideoWindow(Serializer::Properties& props, bool is_derived)
    : Window(props, true),
      m_video_impl(std::make_unique<detail::GstDecoderImpl>(this, box().size())),
      on_position_changed(&m_video_impl->on_position_changed),
      on_error(&m_video_impl->on_error),
      on_eos(&m_video_impl->on_eos),
      on_state_changed(&m_video_impl->on_state_changed)
{
    fill_flags().clear();

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void VideoWindow::draw(Painter& painter, const Rect& rect)
{
    m_video_impl->draw(painter, rect);
}

int64_t VideoWindow::position() const
{
    return m_video_impl->position();
}

int64_t VideoWindow::duration() const
{
    return m_video_impl->duration();
}

bool VideoWindow::media(const std::string& uri)
{
    auto type = detail::resolve_path(uri, m_uri);

    switch (type)
    {
    case detail::SchemeType::network:
        return m_video_impl->media(m_uri);
    case detail::SchemeType::filesystem:
    {
        auto gstpath = "file://" + m_uri;
        return m_video_impl->media(gstpath);
    }
    default:
    {
        throw std::runtime_error("unsupported uri: " + m_uri);
    }
    }

    return false;
}

bool VideoWindow::pause()
{
    return m_video_impl->pause();
}

bool VideoWindow::playing() const
{
    return m_video_impl->playing();
}

bool VideoWindow::play()
{
    return m_video_impl->play();
}

bool VideoWindow::volume(int volume)
{
    m_video_impl->volume(volume);
    return true;
}

int VideoWindow::volume() const
{
    return m_video_impl->volume();
}

bool VideoWindow::seek(int64_t pos)
{
    return m_video_impl->seek(pos);
}

void VideoWindow::scale(float hscale, float vscale)
{
    if (!detail::float_equal(hscale, m_hscale) || !detail::float_equal(vscale,m_vscale))
    {
        if (!plane_window())
        {
            m_hscale = hscale;
            m_vscale = vscale;
            m_video_impl->scale(m_hscale, m_vscale);
        }
        else
        {
            Window::scale(hscale, vscale);
        }
    }
}

void VideoWindow::resize(const Size& size)
{
    if (box().size() != size)
    {
        pause();
        Window::resize(size);
        m_video_impl->resize(size);
        play();
    }
}

bool VideoWindow::has_audio() const
{
    return m_video_impl->has_audio();
}

void VideoWindow::gst_custom_pipeline(const std::string& pipeline_desc)
{
    m_video_impl->custom_pipeline(pipeline_desc);
}

void VideoWindow::loopback(bool enable)
{
    m_video_impl->loopback(enable);
}

bool VideoWindow::loopback() const
{
    return m_video_impl->loopback();
}

void VideoWindow::serialize(Serializer& serializer) const
{
    serializer.add_property("uri", m_uri);
    serializer.add_property("loopback", loopback());
    serializer.add_property("volume", volume());
    Window::serialize(serializer);
}

void VideoWindow::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        switch (detail::hash(std::get<0>(p)))
        {
        case detail::hash("uri"):
        {
            media(std::get<1>(p));
            break;
        }
        case detail::hash("loopback"):
        {
            loopback(detail::from_string(std::get<1>(p)));
            break;
        }
        case detail::hash("volume"):
        {
            volume(std::stoi(std::get<1>(p)));
            break;
        }
        default:
            return false;
        }
        return true;
    }), props.end());
}

VideoWindow::VideoWindow(VideoWindow&&) noexcept = default;
VideoWindow& VideoWindow::operator=(VideoWindow&&) noexcept = default;

VideoWindow::~VideoWindow() noexcept
{
    m_video_impl->destroyPipeline();
}

} //namespace v1

} //namespace egt
