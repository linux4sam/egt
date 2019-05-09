/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_ALIGNMENT_H
#define EGT_DETAIL_ALIGNMENT_H

/**
 * @file
 * @brief Working with alignment.
 */

#include <egt/geometry.h>
#include <egt/widgetflags.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Given an item size, and a bounding box, and an alignment parameter,
 * return the rectangle the item box should be move/resized to.
 *
 * @param[in] orig The default rect to use.
 * @param[in] bounding The bounding box to position inside of.
 * @param[in] align The alignment setting to use for positioning.
 * @param[in] padding Padding to use when positioning.
 * @param[in] horizontal_ratio Horizontal ratio value relative to bounding.
 * @param[in] vertical_ratio Vertical ratio value relative to bounding.
 * @param[in] xratio X ratio value relative to bounding.
 * @param[in] yratio Y ratio value relative to bounding.
 */
Rect align_algorithm(const Rect& orig, const Rect& bounding,
                     alignmask align, default_dim_type padding = 0,
                     default_dim_type horizontal_ratio = 0,
                     default_dim_type vertical_ratio = 0,
                     default_dim_type xratio = 0,
                     default_dim_type yratio = 0);

Rect align_algorithm_force(const Rect& orig, const Rect& bounding,
                           alignmask align, default_dim_type padding = 0,
                           default_dim_type horizontal_ratio = 0,
                           default_dim_type vertical_ratio = 0,
                           default_dim_type xratio = 0,
                           default_dim_type yratio = 0);

/**
 * This is used for aligning two rectangles in a single widget.  For example,
 * a widget that has both text and an image.  The first rectangle is aligned
 * to the main rectangle.  The second rectangle is aligned relative to the
 * first rectangle.
 *
 * @deprecated Use layout mechanics instead.
 */
void double_align(const Rect& main,
                  const Size& fsize, alignmask first_align, Rect& first,
                  const Size& ssize, alignmask second_align, Rect& second,
                  default_dim_type padding = 0);

}
}
}

#endif
