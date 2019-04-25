/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_COLLISION_H
#define EGT_COLLISION_H

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
 */
bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                     const Rect& rhs, shared_cairo_surface_t rimage);

/**
 * Detect alpha collision with a specific point.
 */
bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                     const Point& rhs);

}
}
}

#endif
