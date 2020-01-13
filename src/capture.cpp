/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/capture.h"
#include "detail/camera/gstcaptureimpl.h"

namespace egt
{
inline namespace v1
{
namespace experimental
{

CameraCapture::CameraCapture()
    : CameraCapture("output.avi")
{}


CameraCapture::CameraCapture(const std::string& output,
                             container_type container,
                             PixelFormat format,
                             const std::string& device)
    : m_impl(new detail::CaptureImpl(*this, output, format, container, device))
{}

void CameraCapture::set_output(const std::string& output,
                               container_type container,
                               PixelFormat format)
{
    stop();
    m_impl->set_output(output, container, format);
}

bool CameraCapture::start()
{
    return m_impl->start();
}

std::string CameraCapture::error_message() const
{
    return m_impl->error_message();
}

void CameraCapture::stop()
{
    m_impl->stop();
}

CameraCapture::~CameraCapture() = default;

}
}
}
