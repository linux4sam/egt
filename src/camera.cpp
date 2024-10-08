/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/camera.h"
#include "detail/multimedia/gstdecoderimpl.h"
#include "egt/video.h"

namespace egt
{
inline namespace v1
{

CameraWindow::CameraWindow(const std::string& device,
                           PixelFormat format_hint,
                           WindowHint hint)
    : CameraWindow({}, device, format_hint, hint)
{}

CameraWindow::CameraWindow(const Rect& rect,
                           const std::string& device,
                           PixelFormat format_hint,
                           WindowHint hint)
    : Window(rect, format_hint, detail::check_windowhint(hint)),
      m_camera_impl(std::make_unique<detail::GstDecoderImpl>(this, rect.size())),
      on_error(&m_camera_impl->on_error),
      on_connect(&m_camera_impl->on_connect),
      on_disconnect(&m_camera_impl->on_disconnect)
{
    m_camera_impl->device(device);
}

CameraWindow::CameraWindow(Serializer::Properties& props, bool is_derived)
    : Window(props, true),
      m_camera_impl(std::make_unique<detail::GstDecoderImpl>(this, box().size())),
      on_error(&m_camera_impl->on_error),
      on_connect(&m_camera_impl->on_connect),
      on_disconnect(&m_camera_impl->on_disconnect)
{
    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void CameraWindow::draw(Painter& painter, const Rect& rect)
{
    m_camera_impl->draw(painter, rect);
}

bool CameraWindow::start()
{
    allocate_screen();
    return m_camera_impl->start();
}

bool CameraWindow::playing() const
{
    return m_camera_impl->playing();
}

bool CameraWindow::play()
{
    return m_camera_impl->play();
}

void CameraWindow::device(const std::string& device)
{
    m_camera_impl->device(device);
}

std::string CameraWindow::device() const
{
    return m_camera_impl->device();
}

std::vector<std::string> CameraWindow::list_devices()
{
    return m_camera_impl->list_devices();
}

void CameraWindow::scale(float hscale, float vscale)
{
    if (!detail::float_equal(hscale, m_hscale) || !detail::float_equal(vscale, m_vscale))
    {
        if (!plane_window())
        {
            m_hscale = hscale;
            m_vscale = vscale;
            m_camera_impl->scale(m_hscale, m_vscale);
        }
        else
        {
            Window::scale(hscale, vscale);
        }
    }
}

void CameraWindow::stop()
{
    m_camera_impl->stop();
}

void CameraWindow::serialize(Serializer& serializer) const
{
    serializer.add_property("device", device());
    Window::serialize(serializer);
}

void CameraWindow::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "device")
        {
            device(std::get<1>(p));
            return true;
        }
        return false;
    }), props.end());
}

CameraWindow::~CameraWindow() noexcept = default;

} //namespace v1

} //namespace egt
