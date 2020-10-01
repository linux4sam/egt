/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/camera.h"
#include "detail/camera/gstcameraimpl.h"
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
      m_rect(rect),
      m_camera_impl(std::make_unique<detail::CameraImpl>(*this, rect, device))
{}

void CameraWindow::draw(Painter& painter, const Rect& rect)
{
    m_camera_impl->draw(painter, rect);
}

bool CameraWindow::start()
{
    allocate_screen();
    return m_camera_impl->start();
}

void CameraWindow::scale(float hscale, float vscale)
{
    auto xs = detail::change_if_diff<float>(m_hscale, hscale);
    auto ys = detail::change_if_diff<float>(m_vscale, vscale);

    if (xs || ys)
    {
        if (!plane_window())
        {
            m_camera_impl->scale(m_hscale, m_vscale);
        }
        else
        {
            m_box.size(Size(m_rect.width() * m_hscale, m_rect.height() * m_vscale));
            Window::scale(m_hscale, m_vscale);
        }
    }
}

void CameraWindow::handle(Event& event)
{
    switch (event.id())
    {
    case egt::EventId::pointer_drag_start:
    {
        m_start_offset = box().point();
        break;
    }
    case egt::EventId::pointer_drag:
    case egt::EventId::pointer_drag_stop:
    {
        auto diff = event.pointer().drag_start - event.pointer().point;
        auto p = m_start_offset - egt::Point(diff.x(), diff.y());
        move(p);
        break;
    }
    case EventId::pointer_click:
    {
        on_click.invoke();
        break;
    }
    default:
        break;
    }
}

void CameraWindow::stop()
{
    m_camera_impl->stop();
}

CameraWindow::~CameraWindow() noexcept = default;

} //namespace v1

} //namespace egt
