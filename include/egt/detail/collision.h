/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_COLLISION_H
#define EGT_DETAIL_COLLISION_H

#include <egt/geometry.h>
#include <egt/types.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Detect alpha collision between two images.
 *
 * This tests if any pixel from @b limage inside of @b lhs is opaque at the same
 * time it overlaps with an opaque pixel from @b rimage in @b rhs.
 *
 * @param[in] lhs The origin and size of the limage surface.
 * @param[in] limage The first image surface.
 * @param[in] rhs The origin and size of the rimage surface.
 * @param[in] rimage The second image surface.
 */
bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                     const Rect& rhs, shared_cairo_surface_t rimage);

/**
 * Detect alpha collision with a specific point.
 *
 * This basically tests if the pixel at @rhs is opaque.
 *
 * @b lhs specified the origin with its point(). @b rhs is relative to this
 * origin. @b lhs.size() should be the same size as the limage.
 *
 * @param[in] lhs The origin and size of the limage surface.
 * @param[in] limage The image surface.
 * @param[in] rhs The point to test.
 */
bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                     const Point& rhs);

}
}
}

#endif
