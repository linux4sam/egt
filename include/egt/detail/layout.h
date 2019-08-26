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
class Frame;

namespace detail
{

/**
 * Wrapper around properties for managing a rect by flex_layout().
 */
struct LayoutRect
{
    LayoutRect() = default;

    LayoutRect(uint32_t b,
               const Rect& r,
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
    std::string str;
};

/**
 * Perform a manual flex layout given a parent rect and children rects.
 */
void flex_layout(const Rect& parent,
                 std::vector<LayoutRect>& children,
                 justification justify,
                 orientation orient);

/**
 * Perform a manual flex layout with a parent and child rect, but allow a global
 * child alignment with the @b align parameter.
 */
void flex_layout(const Rect& parent,
                 std::vector<LayoutRect>& children,
                 justification justify,
                 orientation orient,
                 alignmask align);
}
}
}

#endif
