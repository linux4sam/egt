/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_LAYOUT_H
#define EGT_DETAIL_LAYOUT_H

#include <egt/detail/meta.h>
#include <egt/geometry.h>
#include <egt/widgetflags.h>
#include <string>
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
struct EGT_API LayoutRect
{
    /**
     * @param[in] b Behave flags.
     * @param[in] r Current rectangle.
     * @param[in] lm Left margin.
     * @param[in] tm Top margin.
     * @param[in] rm Right margin.
     * @param[in] bm Bottom margin.
     */
    LayoutRect(uint32_t b,
               const Rect& r,
               uint32_t lm = 0,
               uint32_t tm = 0,
               uint32_t rm = 0,
               uint32_t bm = 0) noexcept
        : rect(r),
          behave(b),
          lmargin(lm),
          tmargin(tm),
          rmargin(rm),
          bmargin(bm)
    {}

    /**
     * @param[in] b Behave flags.
     * @param[in] r Current rectangle.
     * @param[in] s String of the object.
     * @param[in] lm Left margin.
     * @param[in] tm Top margin.
     * @param[in] rm Right margin.
     * @param[in] bm Bottom margin.
     */
    LayoutRect(uint32_t b,
               const Rect& r,
               std::string s,
               uint32_t lm = 0,
               uint32_t tm = 0,
               uint32_t rm = 0,
               uint32_t bm = 0) noexcept
        : str(std::move(s)),
          rect(r),
          behave(b),
          lmargin(lm),
          tmargin(tm),
          rmargin(rm),
          bmargin(bm)
    {}

    /// String content of the object.
    std::string str;
    /// Rectangke if the object.
    Rect rect;
    /// Behavior flags of the object.
    uint32_t behave{0};
    /// Left margin.
    uint32_t lmargin{0};
    /// Top margin.
    uint32_t tmargin{0};
    /// Right margin.
    uint32_t rmargin{0};
    /// Bottom margin.
    uint32_t bmargin{0};
};

/**
 * Perform a manual flex layout given a parent rect and children rects.
 */
EGT_API void flex_layout(const Rect& parent,
                         std::vector<LayoutRect>& children,
                         Justification justify,
                         Orientation orient);

/**
 * Perform a manual flex layout with a parent and child rect, but allow a global
 * child alignment with the @b align parameter.
 */
EGT_API void flex_layout(const Rect& parent,
                         std::vector<LayoutRect>& children,
                         Justification justify,
                         Orientation orient,
                         const AlignFlags& align);
}
}
}

#endif
