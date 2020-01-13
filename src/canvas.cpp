/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"

namespace egt
{
inline namespace v1
{

Canvas::Canvas(const Size& size, PixelFormat format) noexcept
    : m_surface(cairo_image_surface_create(detail::cairo_format(format), size.width(), size.height()),
                cairo_surface_destroy),
      m_cr(cairo_create(m_surface.get()), cairo_destroy)
{
}

Canvas::Canvas(const SizeF& size, PixelFormat format) noexcept
    : m_surface(cairo_image_surface_create(detail::cairo_format(format), size.width(), size.height()),
                cairo_surface_destroy),
      m_cr(cairo_create(m_surface.get()), cairo_destroy)
{
}

Canvas::Canvas(const shared_cairo_surface_t& surface) noexcept
    : Canvas(surface, detail::egt_format(cairo_image_surface_get_format(surface.get())))
{
    copy(surface);
}

Canvas::Canvas(const shared_cairo_surface_t& surface, PixelFormat format) noexcept
    : m_surface(cairo_surface_create_similar_image(surface.get(),
                detail::cairo_format(format),
                cairo_image_surface_get_width(surface.get()),
                cairo_image_surface_get_height(surface.get())),
                cairo_surface_destroy),
      m_cr(cairo_create(m_surface.get()), cairo_destroy)
{
    copy(surface);
}

Size Canvas::size() const
{
    if (!m_surface)
        return {};

    return {cairo_image_surface_get_width(m_surface.get()),
            cairo_image_surface_get_height(m_surface.get())};
}

PixelFormat Canvas::format() const
{
    if (!m_surface)
        return PixelFormat::invalid;

    return detail::egt_format(cairo_image_surface_get_format(m_surface.get()));
}

void Canvas::reallocate(const Size& size, PixelFormat format)
{
    m_surface = shared_cairo_surface_t(cairo_image_surface_create(detail::cairo_format(format), size.width(), size.height()),
                                       cairo_surface_destroy);
    m_cr = shared_cairo_t(cairo_create(m_surface.get()), cairo_destroy);

}

void Canvas::reallocate(const SizeF& size, PixelFormat format)
{
    m_surface = shared_cairo_surface_t(cairo_image_surface_create(detail::cairo_format(format), size.width(), size.height()),
                                       cairo_surface_destroy);
    m_cr = shared_cairo_t(cairo_create(m_surface.get()), cairo_destroy);

}

void Canvas::zero()
{
    if (m_surface && m_cr)
    {
        cairo_save(m_cr.get());
        cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_SOURCE);
        cairo_set_source_rgba(m_cr.get(), 0, 0, 0, 0);
        cairo_paint(m_cr.get());
        cairo_restore(m_cr.get());
    }
}

void Canvas::copy(const shared_cairo_surface_t& surface)
{
    cairo_save(m_cr.get());
    cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(m_cr.get(), surface.get(), 0, 0);
    cairo_rectangle(m_cr.get(), 0, 0, cairo_image_surface_get_width(surface.get()),
                    cairo_image_surface_get_height(surface.get()));
    cairo_fill(m_cr.get());
    cairo_restore(m_cr.get());
}

void Canvas::copy(const shared_cairo_surface_t& surface, const RectF& rect)
{
    cairo_save(m_cr.get());
    cairo_set_operator(m_cr.get(), CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(m_cr.get(), surface.get(), -rect.x(), -rect.y());
    cairo_rectangle(m_cr.get(), 0, 0, rect.width(), rect.height());
    cairo_fill(m_cr.get());
    cairo_restore(m_cr.get());
}

}
}
