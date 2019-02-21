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

Rect align_algorithm(const Size& size, const Rect& bounding,
                     alignmask align, int margin)
{
    if (unlikely(align == alignmask::none))
        return Rect(bounding.point(), size);

    auto p = bounding.point();
    auto nsize = size;

    if ((align & alignmask::center) == alignmask::center)
    {
        p.x = bounding.x + (bounding.w / 2) - (size.w / 2);
        p.y = bounding.y + (bounding.h / 2) - (size.h / 2);
    }

    if ((align & alignmask::left) == alignmask::left)
        p.x = bounding.x + margin;
    else if ((align & alignmask::right) == alignmask::right)
        p.x = bounding.x + bounding.w - size.w - margin;

    if ((align & alignmask::top) == alignmask::top)
        p.y = bounding.y + margin;
    else if ((align & alignmask::bottom) == alignmask::bottom)
        p.y = bounding.y + bounding.h - size.h - margin;

    if ((align & alignmask::expand_horizontal) == alignmask::expand_horizontal)
    {
        nsize.w = bounding.w - margin * 2;
        p.x = bounding.x + margin;
    }

    if ((align & alignmask::expand_vertical) == alignmask::expand_vertical)
    {
        nsize.h = bounding.h - margin * 2;
        p.y = bounding.y + margin;
    }

    return Rect(p, nsize);
}

void double_align(const Rect& main,
                  const Size& fsize, alignmask first_align, Rect& first,
                  const Size& ssize, alignmask second_align, Rect& second,
                  int margin)
{
    auto ftarget = detail::align_algorithm(fsize, main, first_align, margin);
    auto starget = detail::align_algorithm(ssize, main, second_align, margin);

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
        starget.x = ftarget.x - starget.w - margin;
        if (starget.x - margin < main.x)
        {
            auto diff = main.x - starget.x + margin;
            starget.x += diff;
            ftarget.x += diff;
        }
    }
    else if ((second_align & alignmask::right) == alignmask::right)
    {
        starget.x = ftarget.right() + margin;
        if (starget.right() + margin > main.right())
        {
            auto diff = starget.right() + margin - main.right();
            starget.x -= diff;
            ftarget.x -= diff;
        }
    }

    if ((second_align & alignmask::top) == alignmask::top)
    {
        starget.y = ftarget.y - starget.h - margin;
        if (starget.y - margin < main.y)
        {
            auto diff = main.y - starget.y + margin;
            starget.y += diff;
            ftarget.y += diff;
        }
    }
    else if ((second_align & alignmask::bottom) == alignmask::bottom)
    {
        starget.y = ftarget.bottom() + margin;
        if (starget.bottom() + margin > main.bottom())
        {
            auto diff = starget.bottom() + margin - main.bottom();
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
