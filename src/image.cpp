/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/serialize.h"
#include "egt/image.h"

namespace egt
{
inline namespace v1
{

Image::Image(const std::string& uri, float scale)
    : m_uri(uri)
{
    if (!uri.empty())
    {
        m_surface = detail::image_cache().get(uri, scale, scale, false);
        assert(cairo_surface_status(m_surface.get()) == CAIRO_STATUS_SUCCESS);

        m_orig_size = Size(std::ceil(cairo_image_surface_get_width(m_surface.get())),
                           std::ceil(cairo_image_surface_get_height(m_surface.get())));
    }
}

Image::Image(const std::string& uri,
             float hscale, float vscale)
    : m_uri(uri)
{
    if (!uri.empty())
    {
        m_surface = detail::image_cache().get(uri, hscale, vscale, false);
        assert(cairo_surface_status(m_surface.get()) == CAIRO_STATUS_SUCCESS);

        m_orig_size = Size(std::ceil(cairo_image_surface_get_width(m_surface.get())),
                           std::ceil(cairo_image_surface_get_height(m_surface.get())));
    }
}

Image::Image(shared_cairo_surface_t surface)
    : m_surface(std::move(surface))
{
    assert(cairo_surface_status(m_surface.get()) == CAIRO_STATUS_SUCCESS);

    m_orig_size = Size(std::ceil(cairo_image_surface_get_width(m_surface.get())),
                       std::ceil(cairo_image_surface_get_height(m_surface.get())));
}

Image::Image(cairo_surface_t* surface)
    : m_surface(cairo_image_surface_create(cairo_image_surface_get_format(surface),
                                           cairo_image_surface_get_width(surface),
                                           cairo_image_surface_get_height(surface)),
                cairo_surface_destroy)
{
    assert(cairo_surface_status(m_surface.get()) == CAIRO_STATUS_SUCCESS);

    m_orig_size = Size(std::ceil(cairo_image_surface_get_width(m_surface.get())),
                       std::ceil(cairo_image_surface_get_height(m_surface.get())));
}

void Image::load(const std::string& uri, float hscale, float vscale)
{
    if (detail::change_if_diff<>(m_uri, uri))
    {
        if (!uri.empty())
        {
            m_surface = detail::image_cache().get(uri, hscale, vscale, false);
            assert(cairo_surface_status(m_surface.get()) == CAIRO_STATUS_SUCCESS);

            m_orig_size = Size(std::ceil(cairo_image_surface_get_width(m_surface.get())),
                               std::ceil(cairo_image_surface_get_height(m_surface.get())));
        }
    }
}

void Image::scale(float hscale, float vscale, bool approximate)
{
    if (m_uri.empty())
        return;

    if (!detail::float_compare(m_hscale, hscale) ||
        !detail::float_compare(m_vscale, vscale))
    {
        m_surface = detail::image_cache().get(m_uri, hscale, vscale, approximate);
        m_hscale = hscale;
        m_vscale = vscale;
    }
}

void Image::copy()
{
    if (!m_surface_local.get())
    {
        auto canvas = Canvas(surface());
        m_surface_local = canvas.surface();
    }
}

Image Image::crop(const RectF& rect)
{
    Canvas canvas(rect.size());
    canvas.copy(m_surface, rect);
    return canvas.surface();
}

void Image::serialize(const std::string& name, detail::Serializer& serializer) const
{
    serializer.add_property(name, m_uri,
    {
        {"hscale", std::to_string(hscale())},
        {"vscale", std::to_string(vscale())}
    });
}

void Image::deserialize(const std::string& name, const std::string& value,
                        const std::map<std::string, std::string>& attrs)
{
    float hscale = 1.0;
    float vscale = 1.0;

    auto h = attrs.find("hscale");
    if (h != attrs.end())
        hscale = std::stod(h->second);

    auto v = attrs.find("vscale");
    if (v != attrs.end())
        vscale = std::stod(v->second);

    load(value, hscale, vscale);
}

}
}
