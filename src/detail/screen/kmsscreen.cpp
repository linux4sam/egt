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

    struct plane_data* m_plane{nullptr};
    uint32_t m_index{};
    bool m_async{false};
};

static KMSScreen* the_kms = nullptr;

std::vector<planeid> KMSScreen::m_used;

KMSScreen::KMSScreen(bool allocate_primary_plane,
                     pixel_format format)
{
    spdlog::info("DRM/KMS Screen ({} buffers)", max_buffers());

    m_fd = drmOpen("atmel-hlcdc", nullptr);
    if (m_fd < 0)
        throw std::runtime_error("unable to open DRM driver");

    m_device = kms_device_open(m_fd);
    if (!m_device)
        throw std::runtime_error("unable to open KMS device");

    if (allocate_primary_plane)
    {
        uint32_t drmformat = detail::drm_format(format);

        m_plane = plane_create_buffered(m_device,
                                        DRM_PLANE_TYPE_PRIMARY,
                                        0,
                                        m_device->screens[0]->width,
                                        m_device->screens[0]->height,
                                        drmformat,
                                        KMSScreen::max_buffers());
        if (!m_plane)
            throw std::runtime_error("unable to create primary plane");

        plane_fb_map(m_plane);
        plane_apply(m_plane);

        SPDLOG_DEBUG("primary plane dumb buffer {},{} {}", plane_width(m_plane),
                     plane_height(m_plane), format);

        init(m_plane->bufs, KMSScreen::max_buffers(),
             Size(plane_width(m_plane), plane_height(m_plane)),
             format);
    }
    else
    {
        m_size = Size(m_device->screens[0]->width,
                      m_device->screens[0]->height);
    }

    m_pool.reset(new FlipThread(m_plane->buffer_count - 1));

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
            num_buffers = std::atoi(getenv("EGT_KMS_BUFFERS"));
        }
    });
    return num_buffers;
}

void KMSScreen::schedule_flip()
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
    SPDLOG_TRACE("request to allocate overlay {} {} {}", size, format, hint);
    struct plane_data* plane = nullptr;

    if (hint == windowhint::software)
        return plane;

    auto overlay_plane_create = [this, &size, format](plane_type type)
    {
        int drm_type = DRM_PLANE_TYPE_OVERLAY;
        switch (type)
        {
        case plane_type::overlay:
            break;
        case plane_type::cursor:
            drm_type = DRM_PLANE_TYPE_CURSOR;
            break;
        case plane_type::primary:
            break;
        }

        /*
         * overlay_index_zorder[num overlay planes][overlay index]
         */
        static std::map<int, std::array<int, 5>> overlay_index_zorder =
        {
            {0, { 0, 0, 0, 0, 0 }},
            {1, { 0, 0, 0, 0, 0 }},
            {2, { 1, 0, 0, 0, 0 }},
            {3, { 2, 0, 1, 0, 0 }},
            {4, { 2, 0, 1, 3, 0 }},
            {5, { 2, 0, 1, 4, 3 }},
        };

        struct plane_data* plane = nullptr;
        auto count = count_planes(type);
        assert(count <= 5);
        for (auto i = 0U; i < count; i++)
        {
            auto id = planeid(overlay_index_zorder[count][i], drm_type);
            if (find(m_used.begin(), m_used.end(), id) != m_used.end())
                continue;

            plane = plane_create_buffered(m_device,
                                          drm_type,
                                          overlay_index_zorder[count][i],
                                          size.width(),
                                          size.height(),
                                          detail::drm_format(format),
                                          KMSScreen::max_buffers());

            if (plane)
            {
                m_used.push_back(id);
                break;
            }
        }

        return plane;
    };

    if (hint == windowhint::overlay)
    {
        plane = overlay_plane_create(plane_type::overlay);
    }
    else if (hint == windowhint::heo_overlay)
    {
        /// @todo No explicit way to choose HEO plane. Just depending on the
        /// requiring the HEO plane for now even though that is not the only
        /// thing different about an HEO plane.  For example, HEO planes are
        /// scaleable and normal planes are not.
        plane = overlay_plane_create(plane_type::overlay);
    }
    else if (hint == windowhint::cursor_overlay)
    {
        plane = overlay_plane_create(plane_type::cursor);
    }

    if (!plane)
    {
        // fallback to overlay plane
        plane = overlay_plane_create(plane_type::overlay);
    }

    if (plane)
    {
        plane_fb_map(plane);
        plane_set_pos(plane, 0, 0);

        SPDLOG_DEBUG("allocated overlay index {} {},{} {} {}", plane->index,
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
        auto id = planeid(plane->index, DRM_PLANE_TYPE_OVERLAY);
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
