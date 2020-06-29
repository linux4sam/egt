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
                           const AlignFlags& align, DefaultDim padding,
                           DefaultDim horizontal_ratio,
                           DefaultDim vertical_ratio,
                           DefaultDim xratio,
                           DefaultDim yratio)
{
    auto p = orig.point();
    auto s = orig.size();

    // if alignment is empty, then don't force inside of bounding
    if (!align.empty())
    {
        if (p.x() < bounding.x())
            p.x(bounding.x());

        if (p.y() < bounding.y())
            p.y(bounding.y());
    }

    if (xratio)
    {
        p.x(bounding.x() + padding +
            static_cast<float>(bounding.width() - padding * 2) *
            (static_cast<float>(xratio) / 100.0f));
    }
    else if (align.is_set(AlignFlag::left))
        p.x(bounding.x() + padding);
    else if (align.is_set(AlignFlag::right))
        p.x(bounding.x() + bounding.width() - orig.size().width() - padding);
    else if (align.is_set(AlignFlag::center_horizontal))
    {
        p.x(bounding.x() + (bounding.width() / 2) - (orig.size().width() / 2));
    }

    if (yratio)
    {
        p.y(bounding.y() + padding +
            static_cast<float>(bounding.height() - padding * 2) *
            (static_cast<float>(yratio) / 100.0f));
    }
    else if (align.is_set(AlignFlag::top))
        p.y(bounding.y() + padding);
    else if (align.is_set(AlignFlag::bottom))
        p.y(bounding.y() + bounding.height() - orig.size().height() - padding);
    else if (align.is_set(AlignFlag::center_vertical))
    {
        p.y(bounding.y() + (bounding.height() / 2) - (orig.size().height() / 2));
    }

    if (horizontal_ratio)
    {
        s.width(static_cast<float>(bounding.width() - padding * 2) *
                (static_cast<float>(horizontal_ratio) / 100.0f));
    }
    else if (align.is_set(AlignFlag::expand_horizontal))
    {
        s.width(bounding.width() - padding * 2);
        p.x(bounding.x() + padding);
    }

    if (vertical_ratio)
    {
        s.height(static_cast<float>(bounding.height() - padding * 2) *
                 (static_cast<float>(vertical_ratio) / 100.0f));
    }
    else if (align.is_set(AlignFlag::expand_vertical))
    {
        s.height(bounding.height() - padding * 2);
        p.y(bounding.y() + padding);
    }

    return Rect(p, s);
}

Rect align_algorithm(const Rect& orig, const Rect& bounding,
                     const AlignFlags& align, DefaultDim padding,
                     DefaultDim horizontal_ratio,
                     DefaultDim vertical_ratio,
                     DefaultDim xratio,
                     DefaultDim yratio)
{
    return align_algorithm_force(orig,
                                 bounding,
                                 align,
                                 padding,
                                 horizontal_ratio,
                                 vertical_ratio,
                                 xratio,
                                 yratio);
}

}
}
}
