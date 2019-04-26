/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_LAYOUT_H
#define EGT_DETAIL_LAYOUT_H

#include <egt/geometry.h>
#include <egt/widgetflags.h>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace detail
{

struct LayoutRect
{
    LayoutRect()
    {}

    LayoutRect(uint32_t b, const Rect& r,
               uint32_t lm = 0,
               uint32_t tm = 0,
               uint32_t rm = 0,
               uint32_t bm = 0)
        : behave(b),
          rect(r),
          lmargin(lm),
          tmargin(tm),
          rmargin(rm),
          bmargin(bm)
    {}

    uint32_t behave{0};
    Rect rect;
    uint32_t lmargin{0};
    uint32_t tmargin{0};
    uint32_t rmargin{0};
    uint32_t bmargin{0};
};

void flex_layout(const Rect& parent,
                 std::vector<LayoutRect>& children,
                 justification justify,
                 orientation orient);

}
}
}

#endif
