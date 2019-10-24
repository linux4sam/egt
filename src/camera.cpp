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
                           pixel_format format_hint,
                           windowhint hint)
    : CameraWindow({}, device, format_hint, hint)
{}

CameraWindow::CameraWindow(const Rect& rect,
                           const std::string& device,
                           pixel_format format_hint,
                           windowhint hint)
    : Window(rect, format_hint, hint),
      m_impl(new detail::CameraImpl(*this, rect, device, detail::is_target_sama5d4()))
{}

void CameraWindow::draw(Painter& painter, const Rect& rect)
{
    m_impl->draw(painter, rect);
}

bool CameraWindow::start()
{
    allocate_screen();
    return m_impl->start();
}

void CameraWindow::set_scale(float scalex, float scaley)
{
    auto xs = detail::change_if_diff<float>(m_scalex, scalex);
    auto ys = detail::change_if_diff<float>(m_scaley, scaley);

    if (xs || ys)
    {
        if (!flags().is_set(Widget::flag::plane_window))
        {
            m_impl->scale(m_scalex, m_scaley);
        }
        else
        {
            Window::set_scale(m_scalex, m_scaley);
        }
    }
}

std::string CameraWindow::error_message() const
{
    return m_impl->error_message();
}

void CameraWindow::stop()
{
    m_impl->stop();
}

CameraWindow::~CameraWindow() = default;

} //namespace v1

} //namespace egt
