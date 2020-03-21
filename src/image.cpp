/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/canvas.h"
#include "egt/detail/image.h"
#include "egt/detail/imagecache.h"
#include "egt/image.h"
#include "egt/serialize.h"

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

Image::Image(const unsigned char* data, size_t len)
{
    m_surface = detail::load_image_from_memory(data, len);

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

    if (!detail::float_equal(m_hscale, hscale) ||
        !detail::float_equal(m_vscale, vscale))
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

void Image::serialize(const std::string& name, Serializer& serializer) const
{
    Serializer::Attributes attrs;

    if (!detail::float_equal(hscale(), 1.0f))
        attrs.emplace_back("hscale", std::to_string(hscale()));

    if (!detail::float_equal(vscale(), 1.0f))
        attrs.emplace_back("vscale", std::to_string(vscale()));

    serializer.add_property(name, m_uri, attrs);
}

void Image::deserialize(const std::string& name, const std::string& value,
                        const Serializer::Attributes& attrs)
{
    float hscale = 1.0;
    float vscale = 1.0;

    const auto h = std::find_if(attrs.begin(), attrs.end(),
    [](const auto & element) { return element.first == "hscale";});
    if (h != attrs.end())
        hscale = std::stod(h->second);

    const auto v = std::find_if(attrs.begin(), attrs.end(),
    [](const auto & element) { return element.first == "vscale";});
    if (v != attrs.end())
        vscale = std::stod(v->second);

    load(value, hscale, vscale);
}

}
}
