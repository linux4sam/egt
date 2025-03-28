/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/svg.h"
#include "egt/canvas.h"
#include "egt/respath.h"
#define RSVG_DISABLE_DEPRECATION_WARNINGS /* for rsvg_handle_get_dimensions() */
#include <librsvg/rsvg.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

static shared_cairo_surface_t load_svg(RsvgHandle* rsvg,
                                       const SizeF& size,
                                       const std::string& id)
{
    RsvgRectangle viewport;
    viewport.x = 0;
    viewport.y = 0;

    if (size.empty())
    {
        /*
         * rsvg_handle_get_intrinsic_size_in_pixels() is not able to extract
         * the size in pixels directly from the intrinsic dimensions of the SVG
         * document if the `width` or `height` are in percentage units (or if
         * they do not exist, in which case the SVG spec mandates that they
         * default to 100%), whereas `rsvg_handle_get_dimensions()` is.
         */
        RsvgDimensionData dim;
        rsvg_handle_get_dimensions(rsvg, &dim);
        viewport.width = dim.width;
        viewport.height = dim.height;
    }
    else
    {
        viewport.width = size.width();
        viewport.height = size.height();
    }

    Size s(std::ceil(viewport.width), std::ceil(viewport.height));
    if (s.empty())
        return {};

    Canvas canvas(s);
    auto cr = canvas.context();

    if (id.empty())
        rsvg_handle_render_document(rsvg, cr.get(), &viewport, nullptr);
    else
        rsvg_handle_render_layer(rsvg, cr.get(), id.c_str(), &viewport, nullptr);

    return canvas.surface();
}

struct RsvgHandle_deleter
{
    void operator()(RsvgHandle* rsvg) { g_object_unref(rsvg); }
};

shared_cairo_surface_t load_svg(const std::string& filename,
                                const SizeF& size,
                                const std::string& id)
{
    std::unique_ptr<RsvgHandle, RsvgHandle_deleter> rsvg(rsvg_handle_new_from_file(resolve_file_path(filename).c_str(), nullptr));

    if (!rsvg)
        throw std::runtime_error("unable to load svg file: " + filename);

    return load_svg(rsvg.get(), size, id);
}

shared_cairo_surface_t load_svg(const unsigned char* data,
                                size_t len,
                                const SizeF& size,
                                const std::string& id)
{
    std::unique_ptr<RsvgHandle, RsvgHandle_deleter> rsvg(rsvg_handle_new_from_data(data, len, nullptr));

    if (!rsvg)
        throw std::runtime_error("unable to load svg file from memory");

    return load_svg(rsvg.get(), size, id);
}
}
}
}
