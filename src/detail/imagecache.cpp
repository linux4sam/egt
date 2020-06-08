/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/dump.h"
#include "egt/detail/image.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/math.h"
#include "egt/respath.h"
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#ifdef HAVE_SIMD
#include "Simd/SimdLib.hpp"
#endif

namespace egt
{
inline namespace v1
{
namespace detail
{

shared_cairo_surface_t ImageCache::get(const std::string& uri,
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

    SPDLOG_DEBUG("image cache miss {} hscale:{} vscale:{}", uri, hscale, vscale);

    shared_cairo_surface_t image;

    if (detail::float_equal(hscale, 1.0f) &&
        detail::float_equal(vscale, 1.0f))
    {
        std::string path;
        auto type = detail::resolve_path(uri, path);

        switch (type)
        {
        case detail::SchemeType::resource:
        {
            image = detail::load_image_from_resource(path);
            break;
        }
        case detail::SchemeType::filesystem:
        {
            image = detail::load_image_from_filesystem(path);
            break;
        }
        case detail::SchemeType::network:
        {
            image = detail::load_image_from_network(path);
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
        shared_cairo_surface_t back = get(uri, 1.0);

        double width = cairo_image_surface_get_width(back.get());
        double height = cairo_image_surface_get_height(back.get());

        detail::code_timer(false, "scale: ", [&]()
        {
            image = scale_surface(back,
                                  width, height,
                                  width * hscale,
                                  height * vscale);
        });
    }

    if (cairo_surface_status(image.get()) != CAIRO_STATUS_SUCCESS)
    {
        throw std::runtime_error(fmt::format(
                                     "cairo: {}: {}", cairo_status_to_string(cairo_surface_status(image.get())), uri));
    }

    m_cache.insert(std::make_pair(nameid, image));

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
shared_cairo_surface_t
ImageCache::scale_surface(const shared_cairo_surface_t& old_surface,
                          float old_width, float old_height,
                          float new_width, float new_height)
{
    cairo_surface_flush(old_surface.get());

    auto new_surface = shared_cairo_surface_t(
                           cairo_surface_create_similar(old_surface.get(),
                                   CAIRO_CONTENT_COLOR_ALPHA,
                                   new_width,
                                   new_height),
                           cairo_surface_destroy);

    auto src = cairo_image_surface_get_data(old_surface.get());
    auto dst = cairo_image_surface_get_data(new_surface.get());

    SimdResizeBilinear(src,
                       old_width, old_height,
                       cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, old_width),
                       dst, new_width, new_height,
                       cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, new_width),
                       4);

    cairo_surface_mark_dirty(new_surface.get());

    return new_surface;
}
#else
shared_cairo_surface_t
ImageCache::scale_surface(const shared_cairo_surface_t& old_surface,
                          float old_width, float old_height,
                          float new_width, float new_height)
{
    auto new_surface = shared_cairo_surface_t(
                           cairo_surface_create_similar(old_surface.get(),
                                   CAIRO_CONTENT_COLOR_ALPHA,
                                   new_width,
                                   new_height),
                           cairo_surface_destroy);
    auto cr = shared_cairo_t(cairo_create(new_surface.get()),
                             cairo_destroy);

    /* Scale *before* setting the source surface (1) */
    cairo_scale(cr.get(),
                new_width / old_width,
                new_height / old_height);
    cairo_set_source_surface(cr.get(), old_surface.get(), 0, 0);

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
