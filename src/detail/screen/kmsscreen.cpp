/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/screen/flipthread.h"
#include "egt/detail/screen/kmsscreen.h"
#include "egt/eventloop.h"
#include "egt/input.h"
#include "egt/widget.h"
#include "egt/window.h"
#include <algorithm>
#include <cairo.h>
#include <cstring>
#include <drm_fourcc.h>
#include <planes/fb.h>
#include <planes/kms.h>
#include <planes/plane.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <xf86drm.h>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

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

static KMSScreen* the_kms = 0;

std::vector<planeid> KMSScreen::m_used;

KMSScreen::KMSScreen(bool primary)
{
    spdlog::info("DRM/KMS Screen ({} buffers)", max_buffers());

    m_fd = drmOpen("atmel-hlcdc", nullptr);
    if (m_fd < 0)
        throw std::runtime_error("unable to open DRM driver");

    m_device = kms_device_open(m_fd);
    if (!m_device)
        throw std::runtime_error("unable to open KMS device");

    if (primary)
    {
        uint32_t format = DRM_FORMAT_RGB565;

        m_plane = plane_create_buffered(m_device,
                                        DRM_PLANE_TYPE_PRIMARY,
                                        0,
                                        m_device->screens[0]->width,
                                        m_device->screens[0]->height,
                                        format,
                                        KMSScreen::max_buffers());
        if (!m_plane)
            throw std::runtime_error("unable to create primary plane");

        plane_fb_map(m_plane);
        plane_apply(m_plane);

        SPDLOG_DEBUG("primary plane dumb buffer {},{}", plane_width(m_plane),
                     plane_height(m_plane));

        init(m_plane->bufs, KMSScreen::max_buffers(),
             plane_width(m_plane), plane_height(m_plane), detail::egt_format(format));
    }
    else
    {
        m_size = Size(m_device->screens[0]->width,
                      m_device->screens[0]->height);
    }

    the_kms = this;
}

uint32_t KMSScreen::max_buffers()
{
    static uint32_t num_buffers = 3;
    static std::once_flag env_flag;
    std::call_once(env_flag, [&]()
    {
        if (getenv("EGT_KMS_BUFFERS"))
        {
            num_buffers = static_cast<spdlog::level::level_enum>(
                              std::atoi(getenv("EGT_KMS_BUFFERS")));
        }
    });
    return num_buffers;
}

void KMSScreen::schedule_flip()
{
    if (m_plane->buffer_count > 1)
    {
        static FlipThread pool(m_plane->buffer_count - 1);
        pool.enqueue(FlipJob(m_plane, m_index));
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

struct plane_data* KMSScreen::allocate_overlay(const Size& size,
        pixel_format format,
        windowhint hint)
{
    SPDLOG_TRACE("allocate plane of size {} {} {}", size, format, hint);
    struct plane_data* plane = nullptr;

    if (hint == windowhint::software)
        return plane;

    if (hint == windowhint::overlay)
    {
        int count = count_planes(plane_type::overlay);
        for (auto i = count - 1; i >= 0; i--)
        {
            auto id  = planeid(i, DRM_PLANE_TYPE_OVERLAY);
            if (find(m_used.begin(), m_used.end(), id) != m_used.end())
                continue;

            plane = plane_create_buffered(m_device,
                                          DRM_PLANE_TYPE_OVERLAY,
                                          i,
                                          size.width,
                                          size.height,
                                          detail::drm_format(format),
                                          KMSScreen::max_buffers());
            if (plane)
            {
                m_used.push_back(id);
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
            if (find(m_used.begin(), m_used.end(), id) != m_used.end())
                continue;

            plane = plane_create_buffered(m_device,
                                          DRM_PLANE_TYPE_OVERLAY,
                                          i,
                                          size.width,
                                          size.height,
                                          detail::drm_format(format),
                                          KMSScreen::max_buffers());
            if (plane)
            {
                m_used.push_back(id);
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
            if (find(m_used.begin(), m_used.end(), id) != m_used.end())
                continue;

            plane = plane_create_buffered(m_device,
                                          DRM_PLANE_TYPE_CURSOR,
                                          i,
                                          size.width,
                                          size.height,
                                          detail::drm_format(format),
                                          1);
            if (plane)
            {
                m_used.push_back(id);
                break;
            }
        }

        if (!plane)
        {
            int cnt = count_planes(plane_type::overlay);
            for (auto i = 0; i < cnt; i++)
            {
                auto id  = planeid(i, DRM_PLANE_TYPE_OVERLAY);
                if (find(m_used.begin(), m_used.end(), id) != m_used.end())
                    continue;

                plane = plane_create_buffered(m_device,
                                              DRM_PLANE_TYPE_OVERLAY,
                                              i,
                                              size.width,
                                              size.height,
                                              detail::drm_format(format),
                                              KMSScreen::max_buffers());
                if (plane)
                {
                    m_used.push_back(id);
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
            if (find(m_used.begin(), m_used.end(), id) != m_used.end())
                continue;

            plane = plane_create_buffered(m_device,
                                          DRM_PLANE_TYPE_OVERLAY,
                                          i,
                                          size.width,
                                          size.height,
                                          detail::drm_format(format),
                                          KMSScreen::max_buffers());
            if (plane)
            {
                m_used.push_back(id);
                break;
            }
        }
    }

    if (plane)
    {
        plane_fb_map(plane);

        plane_set_pos(plane, 0, 0);

        SPDLOG_DEBUG("allocated plane index {} {},{} {} {}", plane->index,
                     plane_width(plane), plane_height(plane),
                     format, plane->type);
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
        if (static_cast<int>(m_device->planes[x]->type) == drmtype)
            total++;
    }
    return total;
}

void KMSScreen::deallocate_overlay(struct plane_data* plane)
{
    if (plane)
    {
        auto id  = planeid(plane->index, DRM_PLANE_TYPE_OVERLAY);
        m_used.erase(std::remove(m_used.begin(), m_used.end(), id), m_used.end());
        plane_free(plane);
    }
}

void KMSScreen::close()
{
    if (m_device)
        kms_device_close(m_device);
    if (m_fd >= 0)
        drmClose(m_fd);
}

KMSScreen::~KMSScreen()
{
    close();
}

}
}
}
