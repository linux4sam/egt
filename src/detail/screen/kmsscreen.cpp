/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPLANES

#include "egt/detail/screen/kmsscreen.h"
#include "egt/eventloop.h"
#include "egt/input.h"
#include "egt/widget.h"
#include "egt/window.h"
#include <cairo.h>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <drm_fourcc.h>
#include <mutex>
#include <planes/fb.h>
#include <planes/kms.h>
#include <planes/plane.h>
#include <pthread.h>
#include <thread>
#include <xf86drm.h>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

static KMSScreen* the_kms = 0;

struct FlipThread
{
    FlipThread()
        : m_stop(false)
    {
        m_thread = std::thread(&FlipThread::run, this);
#if 0
        sched_param sch_params;
        sch_params.sched_priority = sched_get_priority_max(SCHED_RR);
        if (pthread_setschedparam(m_thread.native_handle(), SCHED_RR, &sch_params))
        {
            std::cerr << "Failed to set thread scheduling : " << std::strerror(errno) << std::endl;
        }
#endif
    }

    void run()
    {
        std::function<void()> task;
        while (true)
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);

                while (!m_stop && m_queue.empty())
                    m_condition.wait(lock);

                if (m_stop)
                    return;

                task = m_queue.front();
                m_queue.pop_front();
            }

            task();
        }
    }

    void enqueue(function<void()> job)
    {
        {
            unique_lock<mutex> lock(m_mutex);
            m_queue.push_back(job);

            while (m_queue.size() > 2)
            {
                cout << "too many flip jobs queued" << endl;
                m_queue.pop_front();
            }
        }
        m_condition.notify_one();
    }

    ~FlipThread()
    {
        m_stop = true;
        m_condition.notify_all();
        m_thread.join();
    }

    std::thread m_thread;
    std::deque<function<void()>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_stop;
};

struct FlipJob
{
    explicit FlipJob(struct plane_data* plane, uint32_t index)
        : m_plane(plane), m_index(index)
    {}

    void operator()()
    {
        plane_flip(m_plane, m_index);
    }

    struct plane_data* m_plane;
    uint32_t m_index;
};

KMSScreen::KMSScreen(bool primary)
    : m_index(0)
{
    m_fd = drmOpen("atmel-hlcdc", NULL);
    assert(m_fd >= 0);

    m_device = kms_device_open(m_fd);
    assert(m_device);

    //kms_device_dump(m_device);

    if (primary)
    {
        static const uint32_t NUM_PRIMARY_BUFFERS = 3;
        uint32_t format = DRM_FORMAT_RGB565;

        m_plane = plane_create2(m_device,
                                DRM_PLANE_TYPE_PRIMARY,
                                0,
                                m_device->screens[0]->width,
                                m_device->screens[0]->height,
                                format,
                                NUM_PRIMARY_BUFFERS);

        assert(m_plane);
        plane_fb_map(m_plane);

        plane_apply(m_plane);

        DBG("primary plane dumb buffer " << plane_width(m_plane) << "," <<
            plane_height(m_plane));

        init(m_plane->bufs, NUM_PRIMARY_BUFFERS,
             plane_width(m_plane), plane_height(m_plane), detail::egt_format(format));
    }
    else
    {
        m_size = Size(m_device->screens[0]->width,
                      m_device->screens[0]->height);
    }

    the_kms = this;
}

void KMSScreen::schedule_flip()
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

uint32_t KMSScreen::index()
{
    return m_index;
}

KMSScreen* KMSScreen::instance()
{
    return the_kms;
}

struct planeid
{
    int index;
    int type;

    planeid(int i, int t)
        : index(i), type(t)
    {}
};

inline bool operator==(const planeid& lhs, const planeid& rhs)
{
    return lhs.type == rhs.type &&
           lhs.index == rhs.index;
}

static vector<planeid> used;

struct plane_data* KMSScreen::allocate_overlay(const Size& size,
        pixel_format format,
        windowhint hint)
{
    DBG("allocate plane of size " << size << " " << format << " " << hint);
    struct plane_data* plane = nullptr;

    if (hint == windowhint::software)
        return plane;

    if (hint == windowhint::overlay)
    {
        int count = count_planes(plane_type::overlay);
        for (auto i = count - 1; i >= 0; i--)
        {
            auto id  = planeid(i, DRM_PLANE_TYPE_OVERLAY);
            if (find(used.begin(), used.end(), id) != used.end())
                continue;

            plane = plane_create2(m_device,
                                  DRM_PLANE_TYPE_OVERLAY,
                                  i,
                                  size.w,
                                  size.h,
                                  detail::drm_format(format),
                                  detail::KMSOverlay::NUM_OVERLAY_BUFFERS);
            if (plane)
            {
                used.push_back(id);
                break;
            }
        }
    }
    else if (hint == windowhint::heo_overlay)
    {
        /// @todo No explicit way to choose HEO plane. Just depending on the
        /// requiring the HEO plane for now even though that is not the only
        /// thing different about an HEO plane.  For example, HEO planes are
        /// scaleable and normal planes are not.
        int count = count_planes(plane_type::overlay);
        for (auto i = count - 1; i >= 0; i--)
        {
            auto id  = planeid(i, DRM_PLANE_TYPE_OVERLAY);
            if (find(used.begin(), used.end(), id) != used.end())
                continue;

            plane = plane_create2(m_device,
                                  DRM_PLANE_TYPE_OVERLAY,
                                  i,
                                  size.w,
                                  size.h,
                                  detail::drm_format(format),
                                  detail::KMSOverlay::NUM_OVERLAY_BUFFERS);
            if (plane)
            {
                used.push_back(id);
                break;
            }
        }
    }
    else if (hint == windowhint::cursor_overlay)
    {
        int count = count_planes(plane_type::cursor);
        for (auto i = 0; i < count; i++)
        {
            auto id  = planeid(i, DRM_PLANE_TYPE_OVERLAY);
            if (find(used.begin(), used.end(), id) != used.end())
                continue;

            plane = plane_create2(m_device,
                                  DRM_PLANE_TYPE_CURSOR,
                                  i,
                                  size.w,
                                  size.h,
                                  detail::drm_format(format),
                                  1);
            if (plane)
            {
                used.push_back(id);
                break;
            }
        }

        if (!plane)
        {
            int count = count_planes(plane_type::overlay);
            for (auto i = 0; i < count; i++)
            {
                auto id  = planeid(i, DRM_PLANE_TYPE_OVERLAY);
                if (find(used.begin(), used.end(), id) != used.end())
                    continue;

                plane = plane_create2(m_device,
                                      DRM_PLANE_TYPE_OVERLAY,
                                      i,
                                      size.w,
                                      size.h,
                                      detail::drm_format(format),
                                      detail::KMSOverlay::NUM_OVERLAY_BUFFERS);
                if (plane)
                {
                    used.push_back(id);
                    break;
                }
            }
        }
    }

    if (!plane)
    {
        int count = count_planes(plane_type::overlay);
        for (auto i = count - 1; i >= 0; i--)
        {

            auto id  = planeid(i, DRM_PLANE_TYPE_OVERLAY);
            if (find(used.begin(), used.end(), id) != used.end())
                continue;

            plane = plane_create2(m_device,
                                  DRM_PLANE_TYPE_OVERLAY,
                                  i,
                                  size.w,
                                  size.h,
                                  detail::drm_format(format),
                                  detail::KMSOverlay::NUM_OVERLAY_BUFFERS);
            if (plane)
            {
                used.push_back(id);
                break;
            }
        }
    }

    if (plane)
    {
        plane_fb_map(plane);

        plane_set_pos(plane, 0, 0);

        DBG("allocated plane index " << plane->index << " [" <<
            plane_width(plane) << "," << plane_height(plane) << "] " <<
            format << " " << plane->type);
    }

    return plane;
}

uint32_t KMSScreen::count_planes(plane_type type)
{
    int drmtype = DRM_PLANE_TYPE_PRIMARY;
    switch (type)
    {
    case plane_type::overlay:
        drmtype = DRM_PLANE_TYPE_OVERLAY;
        break;
    case plane_type::cursor:
        drmtype = DRM_PLANE_TYPE_CURSOR;
        break;
    default:
        break;
    }

    uint32_t total = 0;
    for (uint32_t x = 0; x < m_device->num_planes; x++)
    {
        if ((int)m_device->planes[x]->type == drmtype)
            total++;
    }
    return total;
}

void KMSScreen::close()
{
    kms_device_close(m_device);
    drmClose(m_fd);
}

KMSScreen::~KMSScreen()
{
    close();
}

}
}
}

#endif
