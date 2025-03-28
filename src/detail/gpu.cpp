/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/cairoabstraction.h"
#include "detail/gpu.h"
#include "detail/egtlog.h"
#include "egt/detail/meta.h"
#include "egt/surface.h"

#include <stdexcept>

namespace egt
{
inline namespace v1
{
namespace detail
{
static const std::pair<PixelFormat, enum m2d_pixel_format> m2d_formats[] =
{
    {PixelFormat::argb8888, M2D_PF_ARGB8888},
    {PixelFormat::rgb565, M2D_PF_RGB565},
};

static enum m2d_pixel_format m2d_format(PixelFormat format)
{
    for (const auto& i : m2d_formats)
        if (i.first == format)
            return i.second;

    return M2D_PF_ARGB8888;
}

void gpu_init()
{
    m2d_init();
}

void gpu_cleanup()
{
    m2d_cleanup();
}

GPUSurface::GPUObjectId GPUSurface::next_id = 0;

GPUSurface::GPUSurface(Surface* surface, int prime_fd)
    : m_id(next_id++),
      m_surface(surface)
{
    struct m2d_import_desc desc;

    EGTLOG_TRACE("importing GPU object {}: size={},format={}.",
                 m_id, m_surface->size(), m_surface->format());
    memset(&desc, 0, sizeof(desc));
    desc.width = m_surface->width();
    desc.height = m_surface->height();
    desc.format = m2d_format(m_surface->format());
    desc.stride = m_surface->stride();
    desc.fd = prime_fd;
    desc.cpu_addr = m_surface->data();
    m_buf.reset(m2d_import(&desc));
    if (!m_buf)
        throw std::runtime_error("libm2d: cannot import object from PRIME file descriptor.");
}

GPUSurface::GPUSurface(Surface* surface, void** data)
    : m_id(next_id++),
      m_surface(surface)
{
    EGTLOG_TRACE("allocating GPU object {}: size={},format={}.",
                 m_id, m_surface->size(), m_surface->format());
    m_buf.reset(m2d_alloc(m_surface->width(),
                          m_surface->height(),
                          m2d_format(m_surface->format()),
                          m_surface->stride()));
    if (!m_buf)
        throw std::runtime_error("libm2d: cannot allocate memory.");

    *data = m2d_get_data(m_buf.get());
}

m2d_buffer* GPUSurface::tmp_buffer()
{
    if (!m_tmp)
    {
        EGTLOG_TRACE("allocating side buffer for GPU object {}.", m_id);

        /* Force the pixel format to ARGB8888 to allow alpha blending. */
        const auto format = PixelFormat::argb8888;
        m_tmp.reset(m2d_alloc(m_surface->width(),
                              m_surface->height(),
                              m2d_format(format),
                              Surface::stride(format, m_surface->width())));
        if (!m_tmp)
            EGTLOG_WARN("failed to allocate side buffer for GPU object {}.", m_id);
    }

    return m_tmp.get();
}

void GPUSurface::sync_for_cpu()
{
    if (detail::change_if_diff(m_owned_by_gpu, false))
    {
        struct timespec timeout;

        EGTLOG_TRACE("synchronizing GPU object {} for CPU.", m_id);
        clock_gettime(CLOCK_MONOTONIC, &timeout);
        timeout.tv_sec += 1;
        m2d_sync_for_cpu(m_buf.get(), &timeout);
        cairo_surface_mark_dirty(m_surface->impl());
    }
}

void GPUSurface::sync_for_gpu()
{
    if (detail::change_if_diff(m_owned_by_gpu, true))
    {
        EGTLOG_TRACE("synchronizing GPU object {} for GPU.", m_id);
        cpu_flush();
        m2d_sync_for_gpu(m_buf.get());
    }
}

void GPUSurface::flush(bool claimed_by_cpu)
{
    if (m_owned_by_gpu)
        if (claimed_by_cpu)
            sync_for_cpu();
        else
            gpu_flush();
    else
        cpu_flush();
}

void GPUSurface::cpu_flush()
{
    EGTLOG_TRACE("CPU flushing GPU object {}.", m_id);
    cairo_surface_flush(m_surface->impl());
}

void GPUSurface::gpu_flush()
{
    struct timespec timeout;

    EGTLOG_TRACE("GPU flushing GPU object {}.", m_id);
    clock_gettime(CLOCK_MONOTONIC, &timeout);
    timeout.tv_sec += 1;
    m2d_wait(m_buf.get(), &timeout);
}

}
}
}
