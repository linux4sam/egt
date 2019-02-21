/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_WIDGETFLAGS_H
#define EGT_WIDGETFLAGS_H

/**
 * @file
 * @brief Widget flags.
 */

#include <cstdint>
#include <egt/bitmask.h>
#include <iosfwd>
#include <unordered_set>

namespace egt
{
inline namespace v1
{

/**
 * @brief Alignment flags.
 */
enum class alignmask : uint32_t
{
    /** No alignment. */
    none = 0,
    /**
     * Center alignment is a weak alignment both horizontal and
     * vertical. To break one of those dimensions to another
     * alignment, specify it in addiiton to CENTER.  If both
     * are broken, CENTER has no effect.
     */
    center = (1 << 0),
    /** Horizontal alignment. */
    left = (1 << 1),
    /** Horizontal alignment. */
    right = (1 << 2),
    /** Vertical alignment. */
    top = (1 << 3),
    /** Vertical alignment. */
    bottom = (1 << 4),
    /** Expand only horizontally. */
    expand_horizontal = (1 << 5),
    /** Expand only vertically. */
    expand_vertical = (1 << 6),
    /** Don't align, expand. */
    expand = expand_horizontal | expand_vertical,
};

ENABLE_BITMASK_OPERATORS(alignmask)

/**
 * @brief Generic orientation flags.
 */
enum class orientation
{
    horizontal,
    vertical,
};

}
}

#endif
