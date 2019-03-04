/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/detail/screen/kmsoverlay.h"
#include "egt/utils.h"

#include <planes/fb.h>
#include <planes/kms.h>
#include <planes/plane.h>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

KMSOverlay::KMSOverlay(struct plane_data* plane)
    : m_plane(plane),
      m_index(0)
{
    assert(plane);
    if (plane)
    {
        init(plane->bufs, NUM_OVERLAY_BUFFERS,
             plane_width(plane), plane_height(plane), detail::egt_format(plane_format(plane)));
    }
}

void KMSOverlay::resize(const Size& size)
{
    auto ret = plane_fb_reallocate(m_plane,
                                   size.w, size.h, plane_format(m_plane));
    assert(!ret);
    if (!ret)
    {
        plane_fb_map(m_plane);

        init(m_plane->bufs, NUM_OVERLAY_BUFFERS,
             plane_width(m_plane), plane_height(m_plane),
             detail::egt_format(plane_format(m_plane)));
    }
}

void KMSOverlay::hide()
{
    plane_hide(m_plane);
}

void KMSOverlay::show()
{
    plane_apply(m_plane);
}

void* KMSOverlay::raw()
{
    return m_plane->bufs[index()];
}

void KMSOverlay::schedule_flip()
{
    if (m_plane->buffer_count > 1)
    {
#if 0
        static FlipThread pool;
        pool.enqueue(FlipJob(m_plane, m_index));
#else
        plane_flip(m_plane, m_index);
#endif
    }

    if (++m_index >= m_plane->buffer_count)
        m_index = 0;
}

uint32_t KMSOverlay::index()
{
    return m_index;
}

void KMSOverlay::set_position(const DisplayPoint& point)
{
    /*
     * The hardware planes (maybe driver issue) are sensitive to being moved out
     * of the screen further than their width/height in some cases.
     *
     * So, if we are moving further than necessary, just limit the movement.
     * You can't visually tell because the plane is off the screen anyway.
     */

    /// @todo implement fix to above problem

    plane_set_pos(m_plane, point.x, point.y);
}

void KMSOverlay::set_scale(float scale)
{
    plane_set_scale(m_plane, scale);
}

void KMSOverlay::set_pan_size(const Size& size)
{
    plane_set_pan_size(m_plane, size.w, size.h);
}

void KMSOverlay::set_pan_pos(const Point& point)
{
    plane_set_pan_pos(m_plane, point.x, point.y);
}

float KMSOverlay::scale() const
{
    return m_plane->scale;
}

int KMSOverlay::gem()
{
    // TODO: array
    return m_plane->gem_names[0];
}

void KMSOverlay::apply()
{
    plane_apply(m_plane);
}

KMSOverlay::~KMSOverlay()
{
}

}
}
}
