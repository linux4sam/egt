/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/screen/flipthread.h"
#include "egt/detail/screen/kmsoverlay.h"
#include "egt/detail/screen/kmsscreen.h"
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

struct FlipJob
{
    constexpr explicit FlipJob(struct plane_data* plane, uint32_t index, bool async = false) noexcept
        : m_plane(plane), m_index(index), m_async(async)
    {}

    void operator()()
    {
        if (m_async)
            plane_flip_async(m_plane, m_index);
        else
            plane_flip(m_plane, m_index);
    }

    struct plane_data* m_plane {nullptr};
    uint32_t m_index{};
    bool m_async{false};
};

KMSOverlay::KMSOverlay(struct plane_data* plane)
    : m_plane(plane),
      m_index(0)
{
    assert(plane);
    if (plane)
    {
        init(plane->bufs, KMSScreen::max_buffers(),
             Size(plane_width(plane), plane_height(plane)), detail::egt_format(plane_format(plane)));

        m_pool.reset(new FlipThread(m_plane->buffer_count - 1));
    }
}

void KMSOverlay::resize(const Size& size)
{
    auto ret = plane_fb_reallocate(m_plane,
                                   size.width(), size.height(), plane_format(m_plane));
    assert(!ret);
    if (!ret)
    {
        plane_fb_map(m_plane);

        init(m_plane->bufs, KMSScreen::max_buffers(),
             Size(plane_width(m_plane), plane_height(m_plane)),
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
        if (m_async)
            plane_flip_async(m_plane, m_index);
        else
            m_pool->enqueue(FlipJob(m_plane, m_index));
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

    plane_set_pos(m_plane, point.x(), point.y());
}

void KMSOverlay::set_scale(float scalex, float scaley)
{
    /// This is only supported on HEO planes.
    plane_set_scale_independent(m_plane, scalex, scaley);
}

void KMSOverlay::set_pan_size(const Size& size)
{
    plane_set_pan_size(m_plane, size.width(), size.height());
}

void KMSOverlay::set_pan_pos(const Point& point)
{
    plane_set_pan_pos(m_plane, point.x(), point.y());
}

float KMSOverlay::scale_x() const
{
    return plane_scale_x(m_plane);
}

float KMSOverlay::scale_y() const
{
    return plane_scale_y(m_plane);
}

uint32_t KMSOverlay::get_plane_format()
{
    return plane_format(m_plane);
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
