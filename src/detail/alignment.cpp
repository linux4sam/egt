/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/alignment.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

Rect align_algorithm_force(const Rect& orig, const Rect& bounding,
                           alignmask align, default_dim_type padding,
                           default_dim_type horizontal_ratio,
                           default_dim_type vertical_ratio,
                           default_dim_type xratio,
                           default_dim_type yratio)
{
    auto p = orig.point();
    auto s = orig.size();

    if (p.x < bounding.x)
        p.x = bounding.x;

    if (p.y < bounding.y)
        p.y = bounding.y;

    if (xratio)
    {
        p.x = bounding.x + padding +
              static_cast<float>(bounding.w - padding * 2) *
              (static_cast<float>(xratio) / 100.);
    }
    else if ((align & alignmask::left) == alignmask::left)
        p.x = bounding.x + padding;
    else if ((align & alignmask::right) == alignmask::right)
        p.x = bounding.x + bounding.w - orig.size().w - padding;
    else if ((align & alignmask::center_horizontal) == alignmask::center_horizontal)
    {
        p.x = bounding.x + (bounding.w / 2) - (orig.size().w / 2);
    }

    if (yratio)
    {
        p.y = bounding.y + padding +
              static_cast<float>(bounding.h - padding * 2) *
              (static_cast<float>(yratio) / 100.);
    }
    else if ((align & alignmask::top) == alignmask::top)
        p.y = bounding.y + padding;
    else if ((align & alignmask::bottom) == alignmask::bottom)
        p.y = bounding.y + bounding.h - orig.size().h - padding;
    else if ((align & alignmask::center_vertical) == alignmask::center_vertical)
    {
        p.y = bounding.y + (bounding.h / 2) - (orig.size().h / 2);
    }

    if (horizontal_ratio)
    {
        s.w = static_cast<float>(bounding.w - padding * 2) *
              (static_cast<float>(horizontal_ratio) / 100.);
    }
    else if ((align & alignmask::expand_horizontal) == alignmask::expand_horizontal)
    {
        s.w = bounding.w - padding * 2;
        p.x = bounding.x + padding;
    }

    if (vertical_ratio)
    {
        s.h = static_cast<float>(bounding.h - padding * 2) *
              (static_cast<float>(vertical_ratio) / 100.);
    }
    else if ((align & alignmask::expand_vertical) == alignmask::expand_vertical)
    {
        s.h = bounding.h - padding * 2;
        p.y = bounding.y + padding;
    }

    return Rect(p, s);
}

Rect align_algorithm(const Rect& orig, const Rect& bounding,
                     alignmask align, default_dim_type padding,
                     default_dim_type horizontal_ratio,
                     default_dim_type vertical_ratio,
                     default_dim_type xratio,
                     default_dim_type yratio)
{
    /// this needs to be here, but grids don't work when we don't make
    /// sure the object is at least inside bounding
    //if (align == alignmask::none)
    //    return orig;

    return align_algorithm_force(orig,
                                 bounding,
                                 align,
                                 padding,
                                 horizontal_ratio,
                                 vertical_ratio,
                                 xratio,
                                 yratio);
}

void double_align(const Rect& main,
                  const Size& fsize, alignmask first_align, Rect& first,
                  const Size& ssize, alignmask second_align, Rect& second,
                  default_dim_type padding)
{
    auto ftarget = detail::align_algorithm(fsize, main, first_align, padding);
    auto starget = detail::align_algorithm(ssize, main, second_align, padding);

    if ((second_align & alignmask::center) == alignmask::center)
    {
        starget.move_to_center(ftarget.center());
    }

    /*
     * The basic algoithm is to position the first target, then position the
     * second target relative to it.  If that results in the second target
     * going out of bounds of the main rectangle, move both inside the
     * rectangle.
     */
    if ((second_align & alignmask::left) == alignmask::left)
    {
        starget.x = ftarget.x - starget.w - padding;
        if (starget.x - padding < main.x)
        {
            auto diff = main.x - starget.x + padding;
            starget.x += diff;
            ftarget.x += diff;
        }
    }
    else if ((second_align & alignmask::right) == alignmask::right)
    {
        starget.x = ftarget.right() + padding;
        if (starget.right() + padding > main.right())
        {
            auto diff = starget.right() + padding - main.right();
            starget.x -= diff;
            ftarget.x -= diff;
        }
    }

    if ((second_align & alignmask::top) == alignmask::top)
    {
        starget.y = ftarget.y - starget.h - padding;
        if (starget.y - padding < main.y)
        {
            auto diff = main.y - starget.y + padding;
            starget.y += diff;
            ftarget.y += diff;
        }
    }
    else if ((second_align & alignmask::bottom) == alignmask::bottom)
    {
        starget.y = ftarget.bottom() + padding;
        if (starget.bottom() + padding > main.bottom())
        {
            auto diff = starget.bottom() + padding - main.bottom();
            starget.y -= diff;
            ftarget.y -= diff;
        }
    }

    first = ftarget;
    second = starget;
}

}
}
}
