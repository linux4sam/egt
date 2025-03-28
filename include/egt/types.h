/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TYPES_H
#define EGT_TYPES_H

/**
 * @file
 * @brief Common types.
 */

#include <egt/detail/enum.h>
#include <egt/detail/meta.h>
#include <iosfwd>
#include <memory>

namespace egt
{
inline namespace v1
{

/**
 * Supported pixel formats.
 *
 * @note Not all formats are supported by all windows or backends.
 */
enum class PixelFormat
{
    invalid,  ///< Invalid color format
    rgb565,   ///< 16 bpp, 16-bit color
    argb8888, ///< 32 bpp, 24-bit color + 8-bit alpha
    xrgb8888, ///< 32 bpp, 24-bit color
    yuyv,     ///< Packed format with ½ horizontal chroma resolution, also known
    ///< as YUV 4:2:2
    nv21,     ///< YUV 4:2:0 planar image, with 8 bit Y samples, followed by
    ///< interleaved V/U plane with 8bit 2x2 subsampled chroma samples
    yuv420,   ///< Planar format
    yvyu,
    nv61,
    yuy2,     ///< Packed YUY 4:2:2
    uyvy,     ///< Reverse byte order of YUY2
};

/// Enum string conversion map
template<>
EGT_API const std::pair<PixelFormat, char const*> detail::EnumStrings<PixelFormat>::data[11];

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const PixelFormat& format);

/**
 * Internal detail namespace.
 *
 * The detail namespace is considered unstable and private to EGT. Use at your
 * own risk.
 */
namespace detail
{

/**
 * Convert a pixel format to a DRM format.
 */
uint32_t drm_format(PixelFormat format);

/**
 * Convert a DRM format to a pixel format.
 */
PixelFormat egt_format(uint32_t format);

/**
 * Convert a pixel format to a gstreamer format.
 */
std::string gstreamer_format(PixelFormat format);

/**
 * Demangle a C++ symbol into something human readable.
 *
 * @code{.cpp}
 * detail::demangle(typeid(*this).name());
 * @endcode
 *
 * @note This is g++ specific.
 */
EGT_API std::string demangle(const char* name);

}

}
}

#endif
