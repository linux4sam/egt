/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SURFACE_H
#define EGT_SURFACE_H

/**
 * @file
 * @brief Surface interface.
 */

#include <egt/color.h>
#include <egt/detail/meta.h>
#include <egt/geometry.h>
#include <egt/types.h>
#include <functional>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

namespace detail
{
class FrameBuffer;
class InternalSurface;
}

class EGT_API Surface
{
public:
    using ReleaseFunction = std::function<void(void*)>;

    Surface(const Size& size = {}, PixelFormat format = PixelFormat::argb8888);

    Surface(void* data, const ReleaseFunction& release,
            const Size& size, PixelFormat format, DefaultDim stride);

    Surface(const detail::FrameBuffer& fb);

    ~Surface();

    Surface(const Surface&) = delete;
    Surface(Surface&& rhs);

    Surface& operator=(const Surface&) = delete;
    Surface& operator=(Surface&& rhs);

    explicit operator bool() const noexcept { return !empty(); }

    bool empty() const { return m_data == nullptr; }

    void* data() { return m_data; }
    const void* data() const { return m_data; }

    const Size& size() const { return m_size; }
    PixelFormat format() const { return m_format; }
    DefaultDim width() const { return m_size.width(); }
    DefaultDim height() const { return m_size.height(); }
    DefaultDim stride() const { return m_stride; }

    static DefaultDim stride(PixelFormat format, DefaultDim width);

    void write_to_png(const std::string& name) const;

    void flush(bool claimed_by_cpu = false) const;
    void mark_dirty();

    void zero();

    void color_at(const Point& point, const Color& color);
    EGT_NODISCARD Color color_at(const Point& point) const;

    void flood(const Point& point, const Color& color);

    EGT_NODISCARD const detail::InternalSurface& impl() const { return *m_impl; }

    /**
     * Claim the surface for being used by the CPU.
     *
     * Wait for all GPU operations, if any, to complete.
     */
    void sync_for_cpu() const;

protected:
    void* m_data{nullptr};
    ReleaseFunction m_release;
    Size m_size;
    PixelFormat m_format{PixelFormat::invalid};
    DefaultDim m_stride{0};
    std::unique_ptr<detail::InternalSurface> m_impl;
};

}
}

#endif
