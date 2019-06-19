/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/collision.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

bool alpha_collision(const Rect& lhs, shared_cairo_surface_t limage,
                     const Rect& rhs, shared_cairo_surface_t rimage)
{
    if (Rect::intersect(lhs, rhs))
    {
        auto ldata = reinterpret_cast<unsigned int*>(cairo_image_surface_get_data(limage.get()));
        auto rdata = reinterpret_cast<unsigned int*>(cairo_image_surface_get_data(rimage.get()));

        Rect i = Rect::intersection(lhs, rhs);

        for (int y = i.top(); y < i.bottom(); y++)
        {
            for (int x = i.left(); x < i.right(); x++)
            {
                unsigned int l = ldata[static_cast<uint32_t>((x - lhs.left()) + (y - lhs.top()) * lhs.width)];
                unsigned int r = rdata[static_cast<uint32_t>((x - rhs.left()) + (y - rhs.top()) * rhs.width)];

                if ((l >> 24 & 0xff) && (r >> 24 & 0xff))
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
    if (Rect::intersect(lhs, rhs))
    {
        auto ldata = reinterpret_cast<unsigned int*>(cairo_image_surface_get_data(limage.get()));

        for (int y = lhs.top(); y < lhs.bottom(); y++)
        {
            for (int x = lhs.left(); x < lhs.right(); x++)
            {
                uint32_t l = ldata[static_cast<uint32_t>((x - lhs.left()) + (y - lhs.top()) * lhs.width)];

                if (l >> 24 & 0xff)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

}
}
}
