/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_CAPTURE_H
#define EGT_CAPTURE_H

#include <egt/detail/object.h>
#include <egt/detail/signal.h>
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
 * Capture a camera video feed directly to an output file.
 *
 * @code{.cpp}
 * experimental::CameraCapture capture("output.avi");
 * capture.start();
 * ...
 * capture.stop();
 * @endcode
 */
class EGT_API CameraCapture : public egt::detail::Object
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when an error occurs.
     */
    detail::Signal<const std::string&> on_error;
    /** @} */

    /**
     * Output container type.
     */
    enum class ContainerType
    {
        avi,
        mpeg2ts
    };

    CameraCapture();

    /**
     * @param[in] output The output file path.
     * @param[in] container The output container format.
     * @param[in] format The input pixel format.
     * @param[in] device The camera device.
     */
    explicit CameraCapture(const std::string& output,
                           ContainerType container = ContainerType::avi,
                           PixelFormat format = PixelFormat::yuyv,
                           const std::string& device = "/dev/video0");

    EGT_OPS_NOCOPY_MOVE(CameraCapture);
    ~CameraCapture() noexcept override;

    /**
     * Set the output.
     *
     * @warning If this is called while the camera is currently capturing, it
     * will automatically stop the capture.
     *
     * @param[in] output The output file path.
     * @param[in] container The output container format.
     * @param[in] format The input pixel format.
     */
    virtual void set_output(const std::string& output,
                            ContainerType container = ContainerType::avi,
                            PixelFormat format = PixelFormat::yuyv);

    /**
     * Initialize camera pipeline to capture from the camera.
     *
     * @return true on success
     */
    virtual bool start();

    /**
     * Stop camera capture and finish the output.
     */
    virtual void stop();

protected:

    /**
     * Internal capture implementation.
     */
    std::unique_ptr<detail::CaptureImpl> m_impl;

};

}
}
}

#endif
