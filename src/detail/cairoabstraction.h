/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_CAIROABSTRACTION_H
#define EGT_SRC_DETAIL_CAIROABSTRACTION_H

/**
 * @file
 * @brief Cairo abstraction layer.
 */

#include <detail/gpu.h>
#include <egt/painter.h>
#include <egt/types.h>
#include <cairo.h>
#include <memory>

namespace egt
{
inline namespace v1
{

namespace detail
{

template<typename T, void (D)(T*)>
class CairoPointerAbstraction
{
public:
    CairoPointerAbstraction(T* ptr = nullptr)
        : m_ptr(ptr, D)
    {}

    explicit operator bool() const noexcept { return static_cast<bool>(m_ptr); }

    operator T* () const noexcept { return m_ptr.get(); }

    T* get() const noexcept { return m_ptr.get(); }

private:
    std::unique_ptr<T, decltype(D)> m_ptr;
};

using CairoScaledFontAbstraction = CairoPointerAbstraction<cairo_scaled_font_t, cairo_scaled_font_destroy>;

class InternalFont : public CairoScaledFontAbstraction
{
public:
    using CairoScaledFontAbstraction::CairoScaledFontAbstraction;
};

using CairoPatternAbstraction = CairoPointerAbstraction<cairo_pattern_t, cairo_pattern_destroy>;

class InternalPattern : public CairoPatternAbstraction
{
public:
    using CairoPatternAbstraction::CairoPatternAbstraction;
};

using CairoSurfaceAbstraction = CairoPointerAbstraction<cairo_surface_t, cairo_surface_destroy>;

class InternalSurface : public CairoSurfaceAbstraction
{
public:
    using CairoSurfaceAbstraction::CairoSurfaceAbstraction;

#ifdef HAVE_LIBM2D
    InternalSurface(cairo_surface_t* surface, GPUSurface&& gpu_surface)
        : CairoSurfaceAbstraction(surface)
    {
        m_gpu_surface = std::make_unique<detail::GPUSurface>(std::move(gpu_surface));
    }

    bool is_gpu_capable() const { return static_cast<bool>(m_gpu_surface); }

    detail::GPUSurface& gpu_surface() const { return *m_gpu_surface; }

protected:
    std::unique_ptr<detail::GPUSurface> m_gpu_surface;
#endif
};

using CairoAbstraction = CairoPointerAbstraction<cairo_t, cairo_destroy>;

class InternalContext : public CairoAbstraction
{
public:
    InternalContext([[maybe_unused]] Painter& painter, cairo_t* context)
        : CairoAbstraction(context)
#ifndef HAVE_LIBM2D
    {
    }
#else
        , m_gpu_painter(painter)
    {
    }

    detail::GPUPainter& gpu_painter() { return m_gpu_painter; }

protected:
    detail::GPUPainter m_gpu_painter;
#endif
};

/**
 * Convert a Painter::LineCap to a cairo_line_cap_t.
 */
cairo_line_cap_t cairo_line_cap(Painter::LineCap line_cap);

/**
 * Convert a cairo_line_cap_t to a Painter::LineCap.
 */
Painter::LineCap egt_line_cap(cairo_line_cap_t line_cap);

/**
 * Convert a Painter::AntiAlias to a cairo_antialias_t.
 */
cairo_antialias_t cairo_antialias(Painter::AntiAlias antialias);

/**
 * Convert a cairo_antialias_t to a Painter::AntiAlias.
 */
Painter::AntiAlias egt_antialias(cairo_antialias_t antialias);

/**
 * Convert a PixelFormat to a cairo format.
 */
cairo_format_t cairo_format(PixelFormat format);

/**
 * Convert a cairo format to a pixel format.
 */
PixelFormat egt_format(cairo_format_t format);

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

/**
 * Unique pointer for a cairo context.
 */
using unique_cairo_t =
    std::unique_ptr<cairo_t, detail::cairo_t_deleter>;

/**
 * Unique pointer for a cairo surface.
 */
using unique_cairo_surface_t =
    std::unique_ptr<cairo_surface_t, detail::cairo_surface_t_deleter>;

}
}

#endif
