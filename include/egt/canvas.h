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
#include <egt/painter.h>

namespace egt
{

    /**
     * A Canvas manages its own surface and drawing context.
     */
    class Canvas
    {
    public:

        explicit Canvas(const Size& size,
                        cairo_format_t format = CAIRO_FORMAT_ARGB32) noexcept;

        explicit Canvas(shared_cairo_surface_t surface) noexcept;

        Canvas(shared_cairo_surface_t surface, cairo_format_t format) noexcept;

        /**
         * Get the context for the Canvas.
         */
        shared_cairo_t context() const { return m_cr; }

        /**
         * Get the surface for the Canvas.
         */
        shared_cairo_surface_t surface() const { return m_surface; }

        virtual ~Canvas();

    protected:
        shared_cairo_surface_t m_surface;
        shared_cairo_t m_cr;

    private:

        Canvas() = delete;
    };
}

#endif
