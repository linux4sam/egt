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

#include <cairo.h>
#include <iosfwd>
#include <memory>

namespace egt
{
inline namespace v1
{

using shared_cairo_surface_t =
    std::shared_ptr<cairo_surface_t>;

using shared_cairo_t =
    std::shared_ptr<cairo_t>;

using shared_cairo_pattern_t =
    std::shared_ptr<cairo_pattern_t>;

namespace detail
{
/// @private
struct cairo_t_deleter
{
    void operator()(cairo_t* cr) { cairo_destroy(cr); }
};

/// @private
struct cairo_surface_t_deleter
{
    void operator()(cairo_surface_t* surface) { cairo_surface_destroy(surface); }
};
}

using unique_cairo_t =
    std::unique_ptr<cairo_t, detail::cairo_t_deleter>;

using unique_cairo_surface_t =
    std::unique_ptr<cairo_surface_t, detail::cairo_surface_t_deleter>;


/**
 * Supported pixel formats.
 *
 * @note Not all formats are supported by all windows or backends.
 */
enum class pixel_format
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
};

std::ostream& operator<<(std::ostream& os, const pixel_format& format);

/**
 * Internal detail namespace.
 *
 * The detail namespace is considered unstable and private to EGT. Use at your
 * own risk.
 */
namespace detail
{
cairo_format_t cairo_format(pixel_format format);
uint32_t drm_format(pixel_format format);
pixel_format egt_format(uint32_t format);
pixel_format egt_format(cairo_format_t format);
}

}
}

#endif
