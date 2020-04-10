/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_CANVAS_H
#define EGT_CANVAS_H

/**
 * @file
 * @brief Canvas definition.
 */

#include <egt/detail/meta.h>
#include <egt/geometry.h>
#include <egt/types.h>

namespace egt
{
inline namespace v1
{

/**
 * Manages a unique drawing surface and context.
 *
 * This can be created and directly drawn to with Painter.
 *
 * This is useful, for example, if you need a temporary or intermediate
 * buffer for drawing to.
 *
 * @b Example
 * @code{.cpp}
 * Canvas canvas(Size(100,100));
 * Painter painter(canvas.context());
 * ...
 * @endcode
 *
 * @ingroup drawing
 */
class EGT_API Canvas
{
public:

    /**
     * Construct a canvas with the specified format and size.
     *
     * @param[in] size The size of the canvas.
     * @param[in] format The pixel format for the canvas.
     *
     * @note The canvas contents are undefined. zero() can be used to initialize it.
     */
    explicit Canvas(const Size& size,
                    PixelFormat format = PixelFormat::argb8888) noexcept;

    /**
     * Construct a canvas with the specified format and size.
     *
     * @param[in] size The size of the canvas.
     * @param[in] format The pixel format for the canvas.
     *
     * @note The canvas contents are undefined. zero() can be used to initialize it.
     */
    explicit Canvas(const SizeF& size,
                    PixelFormat format = PixelFormat::argb8888) noexcept;

    /**
     * Construct a canvas with an already existing surface.
     *
     * The canvas will create a copy of the supplied surface.
     *
     * @param[in] surface The surface to copy from. A copy will be made.
     */
    explicit Canvas(const shared_cairo_surface_t& surface) noexcept;

    /**
     * Construct a canvas with an already existing surface, and specify the
     * format of the surface the canvas create a copy to.
     *
     * The canvas will create a copy of the supplied surface.
     *
     * @param[in] surface The surface to copy from. A copy will be made.
     * @param[in] format The pixel format for the canvas.
     */
    Canvas(const shared_cairo_surface_t& surface, PixelFormat format) noexcept;

    /**
     * Get the context for the Canvas.
     */
    EGT_NODISCARD shared_cairo_t context() const { return m_cr; }

    /**
     * Get the surface for the Canvas.
     */
    EGT_NODISCARD shared_cairo_surface_t surface() const { return m_surface; }

    /**
     * Get the size of the surface.
     */
    EGT_NODISCARD Size size() const;

    /**
     * Reallocate the canvas to a different size.
     *
     * @param[in] size The size of the canvas.
     * @param[in] format The pixel format for the canvas.
     */
    void reallocate(const Size& size, PixelFormat format = PixelFormat::argb8888);

    /**
     * Reallocate the canvas to a different size.
     *
     * @param[in] size The size of the canvas.
     * @param[in] format The pixel format for the canvas.
     */
    void reallocate(const SizeF& size, PixelFormat format = PixelFormat::argb8888);

    /**
     * Fill the surface with zero (including for transparency channel if
     * applicable).
     *
     * @note This is not done by default when creating a canvas.
     */
    void zero();

    /**
     * Get the format of the surface.
     */
    EGT_NODISCARD PixelFormat format() const;

    /**
     * Create a copy of a surface.
     */
    void copy(const shared_cairo_surface_t& surface);

    void copy(const shared_cairo_surface_t& surface, const RectF& rect);

protected:

    /**
     * The surface of the canvas.
     */
    shared_cairo_surface_t m_surface;

    /**
     * The context of the canvas.
     */
    shared_cairo_t m_cr;
};

}
}

#endif
