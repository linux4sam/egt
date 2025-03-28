/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_SCREEN_FRAMEBUFFER_H
#define EGT_SRC_DETAIL_SCREEN_FRAMEBUFFER_H

#include <egt/geometry.h>
#include <egt/types.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

class FrameBufferInfo
{
public:
    FrameBufferInfo(void* data, int prime_fd)
        : m_data(data),
          m_prime_fd(prime_fd)
    {}

    EGT_NODISCARD void* data() const { return m_data; }
    EGT_NODISCARD int prime_fd() const { return m_prime_fd; }

private:
    void* m_data;
    int m_prime_fd;
};

class FrameBuffer
{
public:
    FrameBuffer(void* data, int prime_fd,
                const Size& size,
                PixelFormat format, DefaultDim stride)
        : m_data(data),
          m_prime_fd(prime_fd),
          m_size(size),
          m_format(format),
          m_stride(stride)
    {}

    EGT_NODISCARD void* data() const { return m_data; }
    EGT_NODISCARD int prime_fd() const { return m_prime_fd; }
    EGT_NODISCARD const Size& size() const { return m_size; }
    EGT_NODISCARD PixelFormat format() const { return m_format; }
    EGT_NODISCARD DefaultDim stride() const { return m_stride; }

private:
    void* m_data;
    int m_prime_fd;
    Size m_size;
    PixelFormat m_format;
    DefaultDim m_stride;
};

}
}
}

#endif
