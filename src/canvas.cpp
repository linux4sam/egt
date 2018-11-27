/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"

namespace egt
{

    Canvas::Canvas(const Size& size, cairo_format_t format)
        : m_surface(cairo_image_surface_create(format, size.w, size.h),
                    cairo_surface_destroy),
          m_cr(cairo_create(m_surface.get()), cairo_destroy)
    {
    }

    Canvas::Canvas(shared_cairo_surface_t surface)
        : Canvas(surface, cairo_image_surface_get_format(surface.get()))
    {
    }

    Canvas::Canvas(shared_cairo_surface_t surface, cairo_format_t format)
        : m_surface(cairo_surface_create_similar_image(surface.get(),
                    format,
                    cairo_image_surface_get_width(surface.get()),
                    cairo_image_surface_get_height(surface.get())),
                    cairo_surface_destroy),
          m_cr(cairo_create(m_surface.get()), cairo_destroy)
    {
    }

    Canvas::~Canvas()
    {
    }

}
