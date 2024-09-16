/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/capture.h"
#include "detail/multimedia/gstdecoderimpl.h"

namespace egt
{
inline namespace v1
{

CameraCapture::CameraCapture(const std::string& output,
                             PixelFormat format,
                             const std::string& device)
    : m_impl(std::make_unique<detail::GstDecoderImpl>(nullptr, Size())),
      on_error(&m_impl->on_error),
      on_connect(&m_impl->on_connect),
      on_disconnect(&m_impl->on_disconnect)
{
    m_impl->media(device);
    m_impl->output(output, Size(320, 240), format);
}

void CameraCapture::set_output(const std::string& output,
                               const Size& size,
                               PixelFormat format)
{
    stop();
    m_impl->output(output, size, format);
}

bool CameraCapture::start()
{
    if (!m_impl->start())
        return false;

    return m_impl->play();
}

void CameraCapture::stop()
{
    m_impl->stop();
}

CameraCapture::~CameraCapture() = default;

}
}
