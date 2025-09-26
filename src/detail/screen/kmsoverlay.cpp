/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/screen/framebuffer.h"
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/detail/screen/kmsscreen.h"
#include <planes/fb.h>
#include <planes/kms.h>
#include <planes/plane.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

KMSOverlay::KMSOverlay(const Size& size, PixelFormat format, WindowHint hint)
    : m_plane(KMSScreen::instance()->allocate_overlay(size, format, hint))
{
    if (!m_plane)
        throw std::runtime_error("failed to allocate plane");

    const auto num_infos = KMSScreen::max_buffers();
    std::vector<detail::FrameBufferInfo> info;
    info.reserve(num_infos);
    for (uint32_t fd = 0; fd < num_infos; ++fd)
        info.emplace_back(m_plane->bufs[fd], m_plane->prime_fds[fd]);

    init(info.data(), info.size(),
         Size(plane_width(m_plane.get()), plane_height(m_plane.get())),
         detail::egt_format(plane_format(m_plane.get())));
}

void KMSOverlay::resize(const Size& size)
{
    auto ret = plane_fb_reallocate(m_plane.get(),
                                   size.width(), size.height(), plane_format(m_plane.get()));
    assert(!ret);
    if (!ret)
    {
        plane_fb_map(m_plane.get());
#ifdef HAVE_LIBM2D
        plane_fb_export(m_plane.get());
#endif

        const auto num_infos = KMSScreen::max_buffers();
        std::vector<detail::FrameBufferInfo> info;
        info.reserve(num_infos);
        for (uint32_t fd = 0; fd < num_infos; ++fd)
            info.emplace_back(m_plane->bufs[fd], m_plane->prime_fds[fd]);

        init(info.data(), info.size(),
             Size(plane_width(m_plane.get()), plane_height(m_plane.get())),
             detail::egt_format(plane_format(m_plane.get())));
    }
}

void KMSOverlay::hide()
{
    plane_hide(m_plane.get());
}

void KMSOverlay::show()
{
    plane_apply(m_plane.get());
}

void* KMSOverlay::raw()
{
    return m_plane->bufs[index()];
}

void KMSOverlay::schedule_flip()
{
    if (m_plane->buffer_count > 1)
    {
        plane_flip(m_plane.get(), m_index);

        if (++m_index >= m_plane->buffer_count)
            m_index = 0;
    }
}

uint32_t KMSOverlay::index()
{
    return m_index;
}

void KMSOverlay::position(const DisplayPoint& point)
{
    /*
     * The hardware planes (maybe driver issue) are sensitive to being moved out
     * of the screen further than their width/height in some cases.
     *
     * So, if we are moving further than necessary, just limit the movement.
     * You can't visually tell because the plane is off the screen anyway.
     */

    /// @todo implement fix to above problem

    plane_set_pos(m_plane.get(), point.x(), point.y());
}

void KMSOverlay::scale(float hscale, float vscale)
{
    /// This is only supported on HEO planes.
    plane_set_scale_independent(m_plane.get(), hscale, vscale);
}

void KMSOverlay::pan_size(const Size& size)
{
    plane_set_pan_size(m_plane.get(), size.width(), size.height());
}

void KMSOverlay::pan_pos(const Point& point)
{
    plane_set_pan_pos(m_plane.get(), point.x(), point.y());
}

float KMSOverlay::hscale() const
{
    return plane_scale_x(m_plane.get());
}

float KMSOverlay::vscale() const
{
    return plane_scale_y(m_plane.get());
}

uint32_t KMSOverlay::get_plane_format()
{
    return plane_format(m_plane.get());
}

int KMSOverlay::gem()
{
    // TODO: array
    return m_plane->gem_names[0];
}

void KMSOverlay::apply()
{
    plane_apply(m_plane.get());
}

void KMSOverlay::rotate(uint32_t degrees)
{
    plane_apply_rotate(m_plane.get(), degrees);
}

}
}
}
