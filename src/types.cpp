/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/types.h"
#include <cairo.h>
#include <iostream>
#include <map>

#ifdef HAVE_LIBDRM
#include <drm_fourcc.h>
#endif

using namespace std;

namespace egt
{
inline namespace v1
{
std::ostream& operator<<(std::ostream& os, const pixel_format& format)
{
    static std::map<pixel_format, std::string> strings;
    if (strings.empty())
    {
#define MAPITEM(p) strings[p] = #p
        MAPITEM(pixel_format::invalid);
        MAPITEM(pixel_format::rgb565);
        MAPITEM(pixel_format::argb8888);
        MAPITEM(pixel_format::xrgb8888);
        MAPITEM(pixel_format::yuyv);
        MAPITEM(pixel_format::nv21);
        MAPITEM(pixel_format::yuv420);
        MAPITEM(pixel_format::yvyu);
        MAPITEM(pixel_format::nv61);
        MAPITEM(pixel_format::yuy2);
        MAPITEM(pixel_format::uyvy);
#undef MAPITEM
    }

    os << strings[format];
    return os;
}

namespace detail
{
static const map<pixel_format, uint32_t> drm_formats =
{
#ifdef HAVE_LIBDRM
    {pixel_format::rgb565, DRM_FORMAT_RGB565},
    {pixel_format::argb8888, DRM_FORMAT_ARGB8888},
    {pixel_format::xrgb8888, DRM_FORMAT_XRGB8888},
    {pixel_format::yuyv, DRM_FORMAT_YUYV},
    {pixel_format::yuv420, DRM_FORMAT_YUV420},
    {pixel_format::nv21, DRM_FORMAT_NV21},
    {pixel_format::yvyu, DRM_FORMAT_YVYU},
    {pixel_format::nv61, DRM_FORMAT_NV61},
    {pixel_format::yuy2, DRM_FORMAT_YUYV},
    {pixel_format::uyvy, DRM_FORMAT_UYVY},
#else
    {pixel_format::rgb565, 0},
    {pixel_format::argb8888, 1},
    {pixel_format::xrgb8888, 2},
    {pixel_format::yuyv, 3},
    {pixel_format::nv21, 4},
    {pixel_format::yuv420, 5},
    {pixel_format::yvyu, 6},
    {pixel_format::nv61, 7},
    {pixel_format::yuy2, 8},
    {pixel_format::uyvy, 9},
#endif
};

static const map<pixel_format, cairo_format_t> cairo_formats =
{
    {pixel_format::rgb565, CAIRO_FORMAT_RGB16_565},
    {pixel_format::argb8888, CAIRO_FORMAT_ARGB32},
    {pixel_format::xrgb8888, CAIRO_FORMAT_RGB24},
};

cairo_format_t cairo_format(pixel_format format)
{
    auto i = cairo_formats.find(format);
    if (i != cairo_formats.end())
        return i->second;

    return CAIRO_FORMAT_INVALID;
}

uint32_t drm_format(pixel_format format)
{
    auto i = drm_formats.find(format);
    if (i != drm_formats.end())
        return i->second;

    return 0;
}

pixel_format egt_format(uint32_t format)
{
    for (auto& i : drm_formats)
        // cppcheck-suppress useStlAlgorithm
        if (i.second == format)
            return i.first;

    return pixel_format::invalid;
}

pixel_format egt_format(cairo_format_t format)
{
    for (auto& i : cairo_formats)
        // cppcheck-suppress useStlAlgorithm
        if (i.second == format)
            return i.first;

    return pixel_format::invalid;
}

std::string gstreamer_format(pixel_format format)
{
    static const std::map<pixel_format, std::string> formats =
    {
        {pixel_format::argb8888, "BGRx"},
        {pixel_format::xrgb8888, "BGRx"},
        {pixel_format::rgb565, "RGB16"},
        {pixel_format::yuv420, "I420"},
        {pixel_format::yuyv, "YUY2"},
        {pixel_format::nv21, "NV21"},
        {pixel_format::yvyu, "YVYU"},
        {pixel_format::nv61, "NV61"},
        {pixel_format::yuy2, "YUY2"},
        {pixel_format::uyvy, "UYVY"},
    };

    const auto i = formats.find(format);
    if (i != formats.end())
        return i->second;

    return {};
}

}

}
}
