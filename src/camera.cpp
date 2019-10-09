/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/app.h"
#include "egt/camera.h"
#include "detail/camera/gstcameraimpl.h"
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/detail/screen/kmsscreen.h"
#include "egt/video.h"
#include <exception>
#include <fstream>
#include <gst/gst.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <thread>

using namespace std;

namespace egt
{
inline namespace v1
{

CameraWindow::CameraWindow(const std::string& device,
                           pixel_format format,
                           windowhint hint)
    : CameraWindow({}, device, format, hint)
{}

CameraWindow::CameraWindow(const Rect& rect,
                           const std::string& device,
                           pixel_format format,
                           windowhint hint)
    : Window(rect, format, hint)
{
    m_cameraImpl.reset(new detail::CameraImpl((*this), rect, device, detail::is_target_sama5d4()));
}

void CameraWindow::draw(Painter& painter, const Rect& rect)
{
    m_cameraImpl->draw(painter, rect);
}

bool CameraWindow::start()
{
    return m_cameraImpl->start();
}

void CameraWindow::set_scale(float scalex, float scaley)
{
    auto xs = detail::change_if_diff<float>(m_scalex, scalex);
    auto ys = detail::change_if_diff<float>(m_scaley, scaley);

    if (xs || ys)
    {
        if (!flags().is_set(Widget::flag::plane_window))
        {
            m_cameraImpl->scale(m_scalex, m_scaley);
        }
        else
        {
            Window::set_scale(m_scalex, m_scaley);
        }
    }
}

std::string CameraWindow::error_message() const
{
    return m_cameraImpl->error_message();
}

void CameraWindow::stop()
{
    m_cameraImpl->stop();
}

} //namespace v1

} //namespace egt
