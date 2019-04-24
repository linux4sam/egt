/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SVG_H
#define EGT_DETAIL_SVG_H

#include <egt/geometry.h>
#include <egt/types.h>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Load an SVG file into a surface.
 *
 * @param[in] filename The path of the SVG file.
 * @param[in] size Optional size of the surface to create and rasterize to.
 * @param[in] id Optional object id to load from within the SVG file.
 */
shared_cairo_surface_t load_svg(const std::string& filename,
                                const SizeF& size = SizeF(),
                                const std::string& id = std::string());

}
}
}

#endif
