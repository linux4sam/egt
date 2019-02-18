/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_ALIGNMENT_H
#define EGT_ALIGNMENT_H

/**
 * @file
 * @brief Working with alignment.
 */

#include <egt/geometry.h>
#include <egt/widget.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Given an item size, and a bounding box, and an alignment parameter,
 * return the rectangle the item box should be move/resized to.
 */
Rect align_algorithm(const Size& size, const Rect& bounding,
                     alignmask align, int margin = 0);

/**
 * This is used for aligning two rectangles in a single widget.  For example,
 * a widget that has both text and an image.  The first rectangle is aligned
 * to the main rectangle.  The second rectangle is aligned relative to the
 * first rectangle.
 */
void double_align(const Rect& main,
                  const Size& fsize, alignmask first_align, Rect& first,
                  const Size& ssize, alignmask second_align, Rect& second,
                  int margin = 0);

}
}
}

#endif
