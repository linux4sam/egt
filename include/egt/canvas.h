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

#include <cairo.h>
#include <egt/geometry.h>
#include <egt/types.h>

namespace egt
{
inline namespace v1
{

/**
 * A Canvas manages its own surface and context.
 *
 * This can be created and directly draw to with Painter.
 *
 * This is useful, for example, if a widget needs a temporary or intermediate
 * buffer for drawing to.
 */
class Canvas
{
public:

    /**
     * Construct a canvas with the specified format and size.
     */
    explicit Canvas(const Size& size,
                    cairo_format_t format = CAIRO_FORMAT_ARGB32) noexcept;

    /**
     * Construct a canvas with an already existing surface.
     *
     * The canvas will create a copy of the supplied surface.
     */
    explicit Canvas(shared_cairo_surface_t surface) noexcept;

    /**
     * Construct a canvas with an already existing surface, and specify the
     * format of the surface the canvas create a copy to.
     *
     * The canvas will create a copy of the supplied surface.
     */
    Canvas(shared_cairo_surface_t surface, cairo_format_t format) noexcept;

    /**
     * Get the context for the Canvas.
     */
    shared_cairo_t context() const { return m_cr; }

    /**
     * Get the surface for the Canvas.
     */
    shared_cairo_surface_t surface() const { return m_surface; }

    virtual ~Canvas() = default;

protected:

    /**
     * Create a copy of a surface.
     */
    void copy(shared_cairo_surface_t surface);

    /**
     * The surface of the canvase.
     */
    shared_cairo_surface_t m_surface;

    /**
     * The context of the canvas.
     */
    shared_cairo_t m_cr;

private:

    Canvas() = delete;
};

}
}

#endif
