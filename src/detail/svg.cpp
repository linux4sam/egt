/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/svg.h"
#include "egt/canvas.h"
#include "egt/respath.h"
#include <librsvg/rsvg.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

static shared_cairo_surface_t load_svg(std::shared_ptr<RsvgHandle>& rsvg,
                                       const SizeF& size,
                                       const std::string& id)
{
    RsvgDimensionData dim;
    rsvg_handle_get_dimensions(rsvg.get(), &dim);

    auto s = size;
    if (s.empty())
        s = SizeF(dim.width, dim.height);

    Canvas canvas(s);
    auto cr = canvas.context();

    auto scaled = s / SizeF(dim.width, dim.height);

    /* Scale *before* setting the source surface (1) */
    cairo_scale(cr.get(),
                scaled.width(),
                scaled.height());

    /* To avoid getting the edge pixels blended with 0 alpha, which would
     * occur with the default EXTEND_NONE. Use EXTEND_PAD for 1.2 or newer (2)
     */
    cairo_pattern_set_extend(cairo_get_source(cr.get()), CAIRO_EXTEND_PAD);

    /* Replace the destination with the source instead of overlaying */
    cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);

    if (id.empty())
        rsvg_handle_render_cairo(rsvg.get(), canvas.context().get());
    else
        rsvg_handle_render_cairo_sub(rsvg.get(), canvas.context().get(), id.c_str());

    return canvas.surface();
}

shared_cairo_surface_t load_svg(const std::string& filename,
                                const SizeF& size,
                                const std::string& id)
{
    std::shared_ptr<RsvgHandle> rsvg(rsvg_handle_new_from_file(resolve_file_path(filename).c_str(), nullptr),
    [](RsvgHandle * r) { g_object_unref(r); });

    if (!rsvg)
        throw std::runtime_error("unable to load svg file: " + filename);

    return load_svg(rsvg, size, id);
}

shared_cairo_surface_t load_svg(const unsigned char* data,
                                size_t len,
                                const SizeF& size,
                                const std::string& id)
{
    std::shared_ptr<RsvgHandle> rsvg(rsvg_handle_new_from_data(data, len, nullptr),
    [](RsvgHandle * r) { g_object_unref(r); });

    if (!rsvg)
        throw std::runtime_error("unable to load svg file from memory");

    return load_svg(rsvg, size, id);
}
}
}
}
