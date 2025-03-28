/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/cairoabstraction.h"
#include "detail/egtlog.h"
#include "detail/gpu.h"
#include "detail/screen/framebuffer.h"
#include "egt/surface.h"
#include "egt/types.h"
#include <deque>

namespace egt
{
inline namespace v1
{

static cairo_surface_t* cairo_surface(Surface& surface)
{
    return cairo_image_surface_create_for_data(
               static_cast<unsigned char*>(surface.data()),
               detail::cairo_format(surface.format()),
               surface.width(),
               surface.height(),
               surface.stride());
}

Surface::Surface(const Size& size, PixelFormat format)
    : m_size(size),
      m_format(format),
      m_stride(stride(format, size.width()))
{
    if (m_size.empty())
    {
        m_data = nullptr;
        return;
    }

    // 'm_data' MUST be set before calling cairo_surface(*this).
#ifdef HAVE_LIBM2D
    try
    {
        detail::GPUSurface gpu_surface(this, &m_data);
        m_impl = std::make_unique<detail::InternalSurface>(cairo_surface(*this),
                 std::move(gpu_surface));
    }
    catch (...)
#endif
    {
        m_data = malloc(height() * stride());
        m_release = free;
        m_impl = std::make_unique<detail::InternalSurface>(cairo_surface(*this));
    }
}

Surface::Surface(void* data, const ReleaseFunction& release,
                 const Size& size, PixelFormat format, DefaultDim stride)
    : m_data(data),
      m_release(release),
      m_size(size),
      m_format(format),
      m_stride(stride)
{
    m_impl = std::make_unique<detail::InternalSurface>(cairo_surface(*this));
}

Surface::Surface(const detail::FrameBuffer& fb)
    : m_data(fb.data()),
      m_size(fb.size()),
      m_format(fb.format()),
      m_stride(fb.stride())
{
    auto* surface = cairo_surface(*this);

#ifdef HAVE_LIBM2D
    try
    {
        m_impl = std::make_unique<detail::InternalSurface>(surface,
                 detail::GPUSurface(this, fb.prime_fd()));
    }
    catch (...)
#endif
    {
        m_impl = std::make_unique<detail::InternalSurface>(surface);
    }
}

Surface::Surface(Surface&& rhs)
    : m_data(rhs.m_data),
      m_release(std::move(rhs.m_release)),
      m_size(rhs.m_size),
      m_format(rhs.m_format),
      m_stride(rhs.m_stride),
      m_impl(std::move(rhs.m_impl))
{
#ifdef HAVE_LIBM2D
    if (impl().is_gpu_capable())
        impl().gpu_surface().owner(this);
#endif

    rhs.m_data = nullptr;
    rhs.m_release = nullptr;
}

Surface& Surface::operator=(Surface&& rhs)
{
    if (&rhs != this)
    {
        if (m_release && m_data)
            m_release(m_data);

        m_data = rhs.m_data;
        m_release = std::move(rhs.m_release);
        m_size = rhs.m_size;
        m_format = rhs.m_format;
        m_stride = rhs.m_stride;
        m_impl = std::move(rhs.m_impl);

#ifdef HAVE_LIBM2D
        if (impl().is_gpu_capable())
            impl().gpu_surface().owner(this);
#endif

        rhs.m_data = nullptr;
        rhs.m_release = nullptr;
    }

    return *this;
}

Surface::~Surface()
{
    if (m_release && m_data)
        m_release(m_data);
}

DefaultDim Surface::stride(PixelFormat format, DefaultDim width)
{
    DefaultDim bits_per_pixel;

    switch (format)
    {
    case PixelFormat::argb8888:
    case PixelFormat::xrgb8888:
        bits_per_pixel = 32;
        break;

    case PixelFormat::rgb565:
        bits_per_pixel = 16;
        break;

    case PixelFormat::a8:
        bits_per_pixel = 8;
        break;

    default:
        bits_per_pixel = 0;
        break;
    }

    /*
     * The least multiple of sizeof(uint32_t) so data are 4-byte aligned:
     * pixman, hence cairo, requires such an alignment.
     */
    return ((width * bits_per_pixel + 0x1f) >> 5) * sizeof(uint32_t);
}

void Surface::write_to_png(const std::string& name) const
{
#if CAIRO_HAS_PNG_FUNCTIONS == 1
    cairo_surface_write_to_png(impl(), name.c_str());
#else
    detail::ignoreparam(name);
    detail::error("png support not available");
#endif
}

void Surface::flush(bool claimed_by_cpu) const
{
#ifdef HAVE_LIBM2D
    if (impl().is_gpu_capable())
    {
        impl().gpu_surface().flush(claimed_by_cpu);
        return;
    }
#else
    detail::ignoreparam(claimed_by_cpu);
#endif
    cairo_surface_flush(impl());
}

void Surface::mark_dirty()
{
    cairo_surface_mark_dirty(impl());
}

void Surface::zero()
{
    flush(true);
    memset(m_data, 0, height() * stride());
    mark_dirty();
}

static inline void set32(void* data, size_t stride, const Point& point, const Color& color) noexcept
{
    *reinterpret_cast<uint32_t*>(reinterpret_cast<unsigned char*>(data) + point.y() * stride + 4 * point.x()) = color.pixel32();
}

static inline void set24(void* data, size_t stride, const Point& point, const Color& color) noexcept
{
    *reinterpret_cast<uint32_t*>(reinterpret_cast<unsigned char*>(data) + point.y() * stride + 4 * point.x()) = color.pixel24();
}

static inline void set16(void* data, size_t stride, const Point& point, const Color& color) noexcept
{
    *reinterpret_cast<uint16_t*>(reinterpret_cast<unsigned char*>(data) + point.y() * stride + 2 * point.x()) = color.pixel16();
}

static inline void set8(void* data, size_t stride, const Point& point, const Color& color) noexcept
{
    *(reinterpret_cast<unsigned char*>(data) + point.y() * stride + point.x()) = color.alpha();
}

void Surface::color_at(const Point& point, const Color& color)
{
    if (!Rect(Point(), size()).intersect(point))
        return;

    flush(true);

    switch (format())
    {
    case PixelFormat::argb8888:
        set32(data(), stride(), point, color);
        break;

    case PixelFormat::xrgb8888:
        set24(data(), stride(), point, color);
        break;

    case PixelFormat::rgb565:
        set16(data(), stride(), point, color);
        break;

    case PixelFormat::a8:
        set8(data(), stride(), point, color);
        break;

    default:
        break;
    }

    mark_dirty();
}

static inline Color get32(const void* data, size_t stride, const Point& point)
{
    return Color::pixel32(*reinterpret_cast<const uint32_t*>(reinterpret_cast<const unsigned char*>(data) + point.y() * stride + 4 * point.x()));
}

static inline Color get24(const void* data, size_t stride, const Point& point)
{
    return Color::pixel24(*reinterpret_cast<const uint32_t*>(reinterpret_cast<const unsigned char*>(data) + point.y() * stride + 4 * point.x()));
}

static inline Color get16(const void* data, size_t stride, const Point& point)
{
    return Color::pixel16(*reinterpret_cast<const uint16_t*>(reinterpret_cast<const unsigned char*>(data) + point.y() * stride + 2 * point.x()));
}

static inline Color get8(const void* data, size_t stride, const Point& point)
{
    return Color(0, 0, 0, *(reinterpret_cast<const unsigned char*>(data) + point.y() * stride + point.x()));
}

Color Surface::color_at(const Point& point) const
{
    if (!Rect(Point(), size()).intersect(point))
        return {};

    flush(true);

    switch (format())
    {
    case PixelFormat::argb8888:
        return get32(data(), stride(), point);

    case PixelFormat::xrgb8888:
        return get24(data(), stride(), point);

    case PixelFormat::rgb565:
        return get16(data(), stride(), point);

    case PixelFormat::a8:
        return get8(data(), stride(), point);

    default:
        break;
    }

    return {};
}

void Surface::flood(const Point& point, const Color& color)
{
    switch (format())
    {
    case PixelFormat::argb8888:
    case PixelFormat::xrgb8888:
    case PixelFormat::rgb565:
        break;
    default:
        // unsupported format
        return;
    }

    if (!Rect(Point(), size()).intersect(point))
        return;

    // flush() is called by Surface::color_at(const Point&)
    const auto target_color = color_at(point);

    // non-recursive breadth first search

    auto check = [&](const Point & p)
    {
        if (!Rect(Point(), size()).intersect(p))
            return false;

        const auto c = color_at(p);
        return c != color && c == target_color;
    };

    std::deque<Point> q;
    q.emplace_back(point);

    while (!q.empty())
    {
        auto p = q.back();
        q.pop_back();

        color_at(p, color);

        if (check(p + Point(1, 0)))
            q.emplace_back(p + Point(1, 0));
        if (check(p - Point(1, 0)))
            q.emplace_back(p - Point(1, 0));
        if (check(p + Point(0, 1)))
            q.emplace_back(p + Point(0, 1));
        if (check(p - Point(0, 1)))
            q.emplace_back(p - Point(0, 1));
    }

    mark_dirty();
}

void Surface::sync_for_cpu() const
{
#ifdef HAVE_LIBM2D
    if (impl().is_gpu_capable())
        impl().gpu_surface().sync_for_cpu();
#endif
}

}
}
