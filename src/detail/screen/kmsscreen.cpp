/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/egtlog.h"
#include "detail/screen/flipthread.h"
#include "detail/screen/framebuffer.h"
#include "egt/detail/screen/kmsscreen.h"
#include "egt/eventloop.h"
#include "egt/input.h"
#include "egt/widget.h"
#include "egt/window.h"
#include <algorithm>
#include <cstring>
#include <drm_fourcc.h>
#include <filesystem>
#include <fstream>
#include <planes/fb.h>
#include <planes/kms.h>
#include <planes/plane.h>
#include <string>
#include <xf86drm.h>

namespace fs = std::filesystem;

namespace egt
{
inline namespace v1
{
namespace detail
{

void plane_t_deleter::operator()(plane_data* plane)
{
    plane_fb_unmap(plane);
    plane_free(plane);
}

struct FlipJob
{
    constexpr explicit FlipJob(struct plane_data* plane,
                               uint32_t index,
                               bool async = false) noexcept
        : m_plane(plane), m_index(index), m_async(async)
    {}

    void operator()()
    {
        if (m_async)
            plane_flip_async(m_plane, m_index);
        else
            plane_flip(m_plane, m_index);
    }

    plane_data* m_plane {nullptr};
    uint32_t m_index{};
    bool m_async{false};
};

static KMSScreen* the_kms = nullptr;

std::vector<planeid> KMSScreen::m_used;

KMSScreen::KMSScreen(bool allocate_primary_plane,
                     PixelFormat format)
{
    detail::info("DRM/KMS Screen ({} buffers)", max_buffers());

    m_fd = drmOpen("atmel-hlcdc", nullptr);
    if (m_fd < 0)
        throw std::runtime_error("unable to open DRM driver");

    m_device = kms_device_open(m_fd);
    if (!m_device)
        throw std::runtime_error("unable to open KMS device");

    if (allocate_primary_plane)
    {
        const auto drmformat = detail::drm_format(format);

        m_plane = unique_plane_t(plane_create_buffered(m_device,
                                 DRM_PLANE_TYPE_PRIMARY,
                                 0,
                                 m_device->screens[0]->width,
                                 m_device->screens[0]->height,
                                 drmformat,
                                 KMSScreen::max_buffers()));
        if (!m_plane)
            throw std::runtime_error("unable to create primary plane");

        plane_fb_map(m_plane.get());
#ifdef HAVE_LIBM2D
        plane_fb_export(m_plane.get());
#endif
        plane_apply(m_plane.get());

        EGTLOG_DEBUG("primary plane dumb buffer {},{} {}", plane_width(m_plane.get()),
                     plane_height(m_plane.get()), format);

        const auto num_infos = KMSScreen::max_buffers();
        std::vector<detail::FrameBufferInfo> info;
        info.reserve(num_infos);
        for (uint32_t fd = 0; fd < num_infos; ++fd)
            info.emplace_back(m_plane->bufs[fd], m_plane->prime_fds[fd]);

        init(info.data(), info.size(),
             Size(plane_width(m_plane.get()), plane_height(m_plane.get())),
             format);

        m_pool = std::make_unique<FlipThread>(m_plane->buffer_count - 1);
    }
    else
    {
        init(nullptr, 0, Size(m_device->screens[0]->width,
                              m_device->screens[0]->height), format);
    }

    assert(!the_kms);
    the_kms = this;
}

uint32_t KMSScreen::max_buffers()
{
    static uint32_t num_buffers = 3;
    static std::once_flag env_flag;
    std::call_once(env_flag, [&]()
    {
        if (getenv("EGT_KMS_BUFFERS") && strlen(getenv("EGT_KMS_BUFFERS")))
        {
            num_buffers = std::stoi(getenv("EGT_KMS_BUFFERS"));
        }
    });
    return num_buffers;
}

void KMSScreen::schedule_flip()
{
    if (m_plane->buffer_count > 1)
    {
        m_pool->enqueue(FlipJob(m_plane.get(), m_index, m_async));

        if (++m_index >= m_plane->buffer_count)
            m_index = 0;
    }
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

plane_data* KMSScreen::overlay_plane_create(const Size& size,
        PixelFormat format,
        plane_type type)
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
    static const std::pair<int, std::array<int, 5>> overlay_index_zorder[] =
    {
        {0, { 0, 0, 0, 0, 0 }},
        {1, { 0, 0, 0, 0, 0 }},
        {2, { 1, 0, 0, 0, 0 }},
        {3, { 2, 0, 1, 0, 0 }},
        {4, { 2, 0, 1, 3, 0 }},
        {5, { 2, 0, 1, 4, 3 }},
    };

    plane_data* plane = nullptr;
    auto count = count_planes(type);
    assert(count <= 5);
    for (auto i = 0U; i < count; i++)
    {
        auto id = planeid(overlay_index_zorder[count].second[i], drm_type);
        if (find(m_used.begin(), m_used.end(), id) != m_used.end())
            continue;

        plane = plane_create_buffered(m_device,
                                      drm_type,
                                      overlay_index_zorder[count].second[i],
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
}

unique_plane_t KMSScreen::allocate_overlay(const Size& size,
        PixelFormat format,
        WindowHint hint)
{
    EGTLOG_TRACE("request to allocate overlay {} {} {}", size, format, hint);
    unique_plane_t plane;

    if (hint == WindowHint::software)
        return plane;

    if (hint == WindowHint::overlay)
    {
        plane = unique_plane_t(overlay_plane_create(size, format, plane_type::overlay));
    }
    else if (hint == WindowHint::heo_overlay)
    {
        /// @todo No explicit way to choose HEO plane. Just depending on the
        /// requiring the HEO plane for now even though that is not the only
        /// thing different about an HEO plane.  For example, HEO planes are
        /// scale-able and normal planes are not.
        plane = unique_plane_t(overlay_plane_create(size, format, plane_type::overlay));
    }
    else if (hint == WindowHint::cursor_overlay)
    {
        plane = unique_plane_t(overlay_plane_create(size, format, plane_type::cursor));
    }

    if (!plane)
    {
        // fallback to overlay plane
        plane = unique_plane_t(overlay_plane_create(size, format, plane_type::overlay));
    }

    if (plane)
    {
        plane_fb_map(plane.get());
#ifdef HAVE_LIBM2D
        plane_fb_export(plane.get());
#endif
        plane_set_pos(plane.get(), 0, 0);

        EGTLOG_DEBUG("allocated overlay index {} {},{} {} {}", plane->index,
                     plane_width(plane.get()), plane_height(plane.get()),
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

void KMSScreen::deallocate_overlay(plane_data* plane)
{
    if (plane)
    {
        auto id = planeid(plane->index, DRM_PLANE_TYPE_OVERLAY);
        m_used.erase(std::remove(m_used.begin(), m_used.end(), id), m_used.end());
    }
}

void KMSScreen::close()
{
    m_pool.reset();
    m_plane.reset();

    if (m_device)
    {
        kms_device_close(m_device);
        m_device = nullptr;
    }

    if (m_fd >= 0)
    {
        drmClose(m_fd);
        m_fd = -1;
    }
}

static fs::path const get_backlight_dir(std::string const& path)
{
    auto const iterator = fs::directory_iterator{fs::path{path}};
    return iterator->path();
}

size_t KMSScreen::max_brightness() const
{
    size_t max_brightness{};
    auto const backlight_dir = get_backlight_dir("/sys/class/backlight/");
    auto const max_backlight_path = backlight_dir / "max_brightness";
    std::ifstream max_brightness_file{max_backlight_path};
    max_brightness_file >> max_brightness;
    return max_brightness;
}

size_t KMSScreen::brightness() const
{
    size_t brightness{};
    auto const backlight_dir = get_backlight_dir("/sys/class/backlight/");
    auto const brightness_backlight_path = backlight_dir / "brightness";
    std::ifstream brightness_file{brightness_backlight_path};
    brightness_file >> brightness;
    return brightness;
}

void KMSScreen::brightness(size_t brightness)
{
    auto max = max_brightness();
    if (brightness > max)
        brightness = max;
    auto const backlight_dir = get_backlight_dir("/sys/class/backlight/");
    std::ofstream{backlight_dir / "brightness"} << std::to_string(brightness);
}

KMSScreen::KMSScreen(KMSScreen&&) noexcept = default;
KMSScreen& KMSScreen::operator=(KMSScreen&&) noexcept = default;

KMSScreen::~KMSScreen() noexcept
{
    close();

    if (the_kms == this)
        the_kms = nullptr;
}

}
}
}
