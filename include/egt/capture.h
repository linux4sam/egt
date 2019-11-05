/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_CAPTURE_H
#define EGT_CAPTURE_H

#include <egt/detail/object.h>
#include <egt/types.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

namespace detail
{
class CaptureImpl;
}

namespace experimental
{

/**
 * Utility to capture a video feed directly to a file.
 *
 */
class CameraCapture : public egt::detail::Object
{
public:

    enum class container_type
    {
        avi,
        mpeg2ts
    };

    CameraCapture() = delete;

    CameraCapture(const std::string& output,
                  container_type container = container_type::avi,
                  pixel_format format = pixel_format::yuv420,
                  const std::string& device = "/dev/video0");

    /**
     * Initialize camera pipeline to capture image feed from the camera.
     *
     * @return true on success
     */
    virtual bool start();

    /**
     * Stop camera.
     */
    virtual void stop();

    /**
     * Get error message.
     */
    std::string error_message() const;

    virtual ~CameraCapture();

protected:

    std::unique_ptr<detail::CaptureImpl> m_impl;

};

}
}
}

#endif
