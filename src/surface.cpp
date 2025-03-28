/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/cairoabstraction.h"
#include "detail/egtlog.h"
#include "egt/surface.h"
#include "egt/types.h"

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
    : m_release(free),
      m_size(size),
      m_format(format),
      m_stride(stride(format, size.width()))
{
    if (m_size.empty())
    {
        m_data = nullptr;
        return;
    }

    m_data = malloc(height() * stride());
    m_impl = std::make_unique<detail::InternalSurface>(cairo_surface(*this));
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

Surface::Surface(Surface&& rhs)
    : m_data(rhs.m_data),
      m_release(std::move(rhs.m_release)),
      m_size(rhs.m_size),
      m_format(rhs.m_format),
      m_stride(rhs.m_stride),
      m_impl(std::move(rhs.m_impl))
{
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

void Surface::flush() const
{
    cairo_surface_flush(impl());
}

void Surface::mark_dirty()
{
    cairo_surface_mark_dirty(impl());
}

void Surface::zero()
{
    flush();
    memset(m_data, 0, height() * stride());
    mark_dirty();
}

}
}
