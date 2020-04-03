/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/detail/enum.h"
#include "egt/types.h"
#include <cairo.h>
#include <ostream>

#ifdef HAVE_LIBDRM
#include <drm_fourcc.h>
#endif

#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#endif

namespace egt
{
inline namespace v1
{
template<>
const std::pair<PixelFormat, char const*> detail::EnumStrings<PixelFormat>::data[] =
{
    {PixelFormat::invalid, "invalid"},
    {PixelFormat::rgb565, "rgb565"},
    {PixelFormat::argb8888, "argb8888"},
    {PixelFormat::xrgb8888, "xrgb8888"},
    {PixelFormat::yuyv, "yuyv"},
    {PixelFormat::nv21, "nv21"},
    {PixelFormat::yuv420, "yuv420"},
    {PixelFormat::yvyu, "yvyu"},
    {PixelFormat::nv61, "nv61"},
    {PixelFormat::yuy2, "yuy2"},
    {PixelFormat::uyvy, "uyvy"},
};

std::ostream& operator<<(std::ostream& os, const PixelFormat& format)
{
    return os << detail::enum_to_string(format);
}

namespace detail
{
static constexpr std::pair<PixelFormat, cairo_format_t> cairo_formats[] =
{
    {PixelFormat::rgb565, CAIRO_FORMAT_RGB16_565},
    {PixelFormat::argb8888, CAIRO_FORMAT_ARGB32},
    {PixelFormat::xrgb8888, CAIRO_FORMAT_RGB24},
};

cairo_format_t cairo_format(PixelFormat format)
{
    for (const auto& i : cairo_formats)
        if (i.first == format)
            return i.second;

    return CAIRO_FORMAT_INVALID;
}

static constexpr std::pair<PixelFormat, uint32_t> drm_formats[] =
{
#ifdef HAVE_LIBDRM
    {PixelFormat::rgb565, DRM_FORMAT_RGB565},
    {PixelFormat::argb8888, DRM_FORMAT_ARGB8888},
    {PixelFormat::xrgb8888, DRM_FORMAT_XRGB8888},
    {PixelFormat::yuyv, DRM_FORMAT_YUYV},
    {PixelFormat::yuv420, DRM_FORMAT_YUV420},
    {PixelFormat::nv21, DRM_FORMAT_NV21},
    {PixelFormat::yvyu, DRM_FORMAT_YVYU},
    {PixelFormat::nv61, DRM_FORMAT_NV61},
    {PixelFormat::yuy2, DRM_FORMAT_YUYV},
    {PixelFormat::uyvy, DRM_FORMAT_UYVY},
#else
    {PixelFormat::rgb565, 0},
    {PixelFormat::argb8888, 1},
    {PixelFormat::xrgb8888, 2},
    {PixelFormat::yuyv, 3},
    {PixelFormat::nv21, 4},
    {PixelFormat::yuv420, 5},
    {PixelFormat::yvyu, 6},
    {PixelFormat::nv61, 7},
    {PixelFormat::yuy2, 8},
    {PixelFormat::uyvy, 9},
#endif
};

uint32_t drm_format(PixelFormat format)
{
    for (const auto& i : drm_formats)
        if (i.first == format)
            return i.second;

    return 0;
}

PixelFormat egt_format(uint32_t format)
{
    for (const auto& i : drm_formats)
        // cppcheck-suppress useStlAlgorithm
        if (i.second == format)
            return i.first;

    return PixelFormat::invalid;
}

PixelFormat egt_format(cairo_format_t format)
{
    for (const auto& i : cairo_formats)
        // cppcheck-suppress useStlAlgorithm
        if (i.second == format)
            return i.first;

    return PixelFormat::invalid;
}

static constexpr std::pair<PixelFormat, const char*> gstreamer_formats[] =
{
    {PixelFormat::argb8888, "BGRx"},
    {PixelFormat::xrgb8888, "BGRx"},
    {PixelFormat::rgb565, "RGB16"},
    {PixelFormat::yuv420, "I420"},
    {PixelFormat::yuyv, "YUY2"},
    {PixelFormat::nv21, "NV21"},
    {PixelFormat::yvyu, "YVYU"},
    {PixelFormat::nv61, "NV61"},
    {PixelFormat::yuy2, "YUY2"},
    {PixelFormat::uyvy, "UYVY"},
};

std::string gstreamer_format(PixelFormat format)
{
    for (const auto& i : gstreamer_formats)
        if (i.first == format)
            return i.second;

    return {};
}

#ifdef __GNUG__
std::string demangle(const char* name)
{
    int status = -4;

    std::unique_ptr<char, void(*)(void*)> res{abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};
    return (status == 0) ? res.get() : name;
}
#else
// does nothing if not g++
std::string demangle(const char* name)
{
    return name;
}
#endif

}

}
}
