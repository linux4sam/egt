/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/collision.h"
#include <cassert>

namespace egt
{
inline namespace v1
{
namespace detail
{

bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                     const Rect& rhs, shared_cairo_surface_t rimage)
{
    if (lhs.intersect(rhs))
    {
        assert(cairo_image_surface_get_format(limage.get()) == CAIRO_FORMAT_ARGB32);
        assert(cairo_image_surface_get_format(rimage.get()) == CAIRO_FORMAT_ARGB32);

        const auto ldata = reinterpret_cast<unsigned int*>(cairo_image_surface_get_data(limage.get()));
        const auto rdata = reinterpret_cast<unsigned int*>(cairo_image_surface_get_data(rimage.get()));
        const auto lpitch = cairo_image_surface_get_stride(limage.get()) / sizeof(uint32_t);
        const auto rpitch = cairo_image_surface_get_stride(rimage.get()) / sizeof(uint32_t);
        const auto i = Rect::intersection(lhs, rhs);

        for (auto y = i.top(); y < i.bottom(); y++)
        {
            for (auto x = i.left(); x < i.right(); x++)
            {
                auto l = ldata[static_cast<uint32_t>((x - lhs.left()) + (y - lhs.top()) * lpitch)];
                auto r = rdata[static_cast<uint32_t>((x - rhs.left()) + (y - rhs.top()) * rpitch)];

                if (((l >> 24) & 0xff) && ((r >> 24) & 0xff))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                     const Point& rhs)
{
    if (lhs.intersect(rhs))
    {
        assert(cairo_image_surface_get_format(limage.get()) == CAIRO_FORMAT_ARGB32);

        const auto ldata = reinterpret_cast<unsigned int*>(cairo_image_surface_get_data(limage.get()));
        const auto pitch = cairo_image_surface_get_stride(limage.get()) / sizeof(uint32_t);

        const auto l = ldata[static_cast<uint32_t>((rhs.x() - lhs.left()) + (rhs.y() - lhs.top()) * pitch)];

        if ((l >> 24) & 0xff)
        {
            return true;
        }
    }

    return false;
}

}
}
}
