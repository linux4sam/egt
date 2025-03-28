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
        float hscale, float vscale, bool approximate)
{
    if (approximate)
    {
        hscale = ImageCache::round(hscale, 0.01);
        vscale = ImageCache::round(vscale, 0.01);
    }

    const auto nameid = id(uri, hscale, vscale);

    auto i = m_cache.find(nameid);
    if (i != m_cache.end())
        return i->second;

    EGTLOG_DEBUG("image cache miss {} hscale:{} vscale:{}", uri, hscale, vscale);

    std::shared_ptr<Surface> image;

    if (detail::float_equal(hscale, 1.0f) &&
        detail::float_equal(vscale, 1.0f))
    {
        std::string path;
        auto type = detail::resolve_path(uri, path);

        switch (type)
        {
        case detail::SchemeType::resource:
        {
            image = std::make_shared<Surface>(detail::load_image_from_resource(path));
            break;
        }
        case detail::SchemeType::filesystem:
        {
            image = std::make_shared<Surface>(detail::load_image_from_filesystem(path));
            break;
        }
        case detail::SchemeType::network:
        {
            image = std::make_shared<Surface>(detail::load_image_from_network(path));
            break;
        }
        default:
        {
            throw std::runtime_error("unsupported uri: " + uri);
        }
        }
    }
    else
    {
        auto back = get(uri, 1.0);

        DefaultDim width = std::round(hscale * back->width());
        DefaultDim height = std::round(vscale * back->height());

        detail::code_timer(false, "scale: ", [&]()
        {
            image = std::make_shared<Surface>(resize(*back, Size(width, height)));
        });
    }

    if (!image)
    {
        throw std::runtime_error(fmt::format("unable to load image: {}", uri));
    }

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
