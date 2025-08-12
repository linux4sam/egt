/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/cairoabstraction.h"
#include "detail/dump.h"
#include "detail/egtlog.h"
#include "egt/detail/image.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/math.h"
#include "egt/respath.h"

#ifdef HAVE_SIMD
#include "Simd/SimdLib.hpp"
#endif

namespace egt
{
inline namespace v1
{
namespace detail
{

std::shared_ptr<Surface> ImageCache::get(const std::string& uri,
        float hscale, float vscale, bool approximate, bool is_cached)
{
    if (approximate)
    {
        hscale = ImageCache::round(hscale, 0.01);
        vscale = ImageCache::round(vscale, 0.01);
    }

    if (egt_unlikely(detail::float_equal(hscale, 0.0f) ||
                     detail::float_equal(vscale, 0.0f) ||
                     hscale < 0 ||
                     vscale < 0))
        throw std::runtime_error(fmt::format("invalid scale factors for image {}: hscale={}, vscale={}",
                                             uri, hscale, vscale));

    const auto nameid = id(uri, hscale, vscale);

    if (egt_unlikely(is_cached))
    {
        auto i = m_cache.find(nameid);
        if (i != m_cache.end())
            return i->second;
    }

    EGTLOG_DEBUG("image cache miss {} hscale:{} vscale:{}", uri, hscale, vscale);

    std::shared_ptr<Surface> image;
    std::string path;
    auto type = detail::resolve_path(uri, path);

    switch (type)
    {
    case detail::SchemeType::resource:
        image = std::make_shared<Surface>(detail::load_image_from_resource(path, hscale, vscale));
        break;

    case detail::SchemeType::filesystem:
        image = std::make_shared<Surface>(detail::load_image_from_filesystem(path, hscale, vscale));
        break;

    case detail::SchemeType::network:
        image = std::make_shared<Surface>(detail::load_image_from_network(path, hscale, vscale));
        break;

    default:
        throw std::runtime_error("unsupported uri: " + uri);
    }

    if (!image)
    {
        throw std::runtime_error(fmt::format("unable to load image: {}", uri));
    }

    if (egt_unlikely(is_cached))
        m_cache.emplace(nameid, image);

    return image;
}

void ImageCache::clear()
{
    m_cache.clear();
}

float ImageCache::round(float v, float fraction)
{
    return floorf(v) + floorf((v - floorf(v)) / fraction) * fraction;
}

std::string ImageCache::id(const std::string& name, float hscale, float vscale)
{
    return fmt::format("{}-{}-{}", name, hscale * 100, vscale * 100);
}

#ifdef HAVE_SIMD
Surface ImageCache::resize(const Surface& surface, const Size& size)
{
    Surface new_surface(size, surface.format());

    SimdResizeBilinear(static_cast<const uint8_t*>(surface.data()),
                       surface.width(),
                       surface.height(),
                       surface.stride(),
                       static_cast<uint8_t*>(new_surface.data()),
                       new_surface.width(),
                       new_surface.height(),
                       new_surface.stride(),
                       4);

    new_surface.mark_dirty();

    return new_surface;
}
#else
Surface ImageCache::resize(const Surface& surface, const Size& size)
{
    Surface new_surface(size, surface.format());
    unique_cairo_t cr(cairo_create(new_surface.impl()));

    /* Scale *before* setting the source surface (1) */
    cairo_scale(cr.get(),
                static_cast<double>(size.width()) / surface.width(),
                static_cast<double>(size.height()) / surface.height());

    cairo_set_source_surface(cr.get(), surface.impl(), 0, 0);

    /* To avoid getting the edge pixels blended with 0 alpha, which would
     * occur with the default EXTEND_NONE. Use EXTEND_PAD for 1.2 or newer (2)
     */
    cairo_pattern_set_extend(cairo_get_source(cr.get()), CAIRO_EXTEND_REFLECT);

    /* Replace the destination with the source instead of overlaying */
    cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);

    /* Do the actual drawing */
    cairo_paint(cr.get());

    return new_surface;
}
#endif

ImageCache& image_cache()
{
    static ImageCache cache;
    return cache;
}

}
}
}
