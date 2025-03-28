/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_GPU_H
#define EGT_SRC_DETAIL_GPU_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/detail/meta.h"

#include <cairo.h>
#ifdef HAVE_LIBM2D
#include <m2d/m2d.h>
#endif

namespace egt
{
inline namespace v1
{

class Surface;

namespace detail
{

#ifndef HAVE_LIBM2D
static inline void gpu_init() {}
static inline void gpu_cleanup() {}
#else
void gpu_init();
void gpu_cleanup();

class GPUSurface
{
public:

    GPUSurface(Surface* surface, int prime_fd);
    GPUSurface(Surface* surface, void** data);

    GPUSurface(const GPUSurface&) = delete;
    GPUSurface(GPUSurface&& rhs) noexcept = default;

    GPUSurface& operator=(const GPUSurface&) = delete;
    GPUSurface& operator=(GPUSurface&& rhs) noexcept = default;

    operator m2d_buffer* () const { return m_buf.get(); }

    m2d_buffer* tmp_buffer();

    using GPUObjectId = int64_t;
    EGT_NODISCARD GPUObjectId id() const { return m_id; }

    void sync_for_cpu();
    void sync_for_gpu();
    void flush(bool claimed_by_cpu);

    /* Only called from Surface(Surface&&) or Surface::operator=(Surface&&). */
    void owner(Surface* surface) { m_surface = surface; }
    EGT_NODISCARD Surface& owner() const { return *m_surface; }

private:

    struct m2d_buffer_deleter
    {
        void operator()(m2d_buffer* buf) { m2d_free(buf); }
    };

    void cpu_flush();
    void gpu_flush();

    GPUObjectId m_id{-1};
    Surface* m_surface{nullptr};
    std::unique_ptr<m2d_buffer, m2d_buffer_deleter> m_buf;
    std::unique_ptr<m2d_buffer, m2d_buffer_deleter> m_tmp;
    bool m_owned_by_gpu{true};

    static GPUObjectId next_id;
};

#endif /* HAVE_LIBM2D */

}
}
}

#endif
