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
 * Flags used for various widget properties.
 */
enum class widgetflag : uint32_t
{
    /**
     * This is an overlay plane window.
     */
    PLANE_WINDOW,

    /**
     * This is a window widget.
     */
    WINDOW,

    /**
     * This is a frame.
     */
    FRAME,

    /**
     * Grab related mouse events.
     *
     * For example, if a button is pressed with the eventid::MOUSE_DOWN
     * event, make sure the button gets subsequent mouse events, including
     * the eventid::MOUSE_UP event.
     */
    GRAB_MOUSE,

    /**
     * Default window flags.
     */
    WINDOW_DEFAULT = WINDOW,
};

namespace detail
{
struct widgetflag_hash
{
    std::size_t operator()(widgetflag const& s) const noexcept
    {
        return std::hash<int> {}(static_cast<int>(s));
    }
};
}

using widgetflags = std::unordered_set<widgetflag, detail::widgetflag_hash>;

std::ostream& operator<<(std::ostream& os, const widgetflags& flags);

/**
 * Alignment flags.
 */
enum class alignmask : uint32_t
{
    /** No alignment. */
    NONE = 0,
    /**
     * Center alignment is a weak alignment both horizontal and
     * vertical. To break one of those dimensions to another
     * alignment, specify it in addiiton to CENTER.  If both
     * are broken, CENTER has no effect.
     */
    CENTER = (1 << 0),
    /** Horizontal alignment. */
    LEFT = (1 << 1),
    /** Horizontal alignment. */
    RIGHT = (1 << 2),
    /** Vertical alignment. */
    TOP = (1 << 3),
    /** Vertical alignment. */
    BOTTOM = (1 << 4),
    /** Expand only horizontally. */
    EXPAND_HORIZONTAL = (1 << 5),
    /** Expand only vertically. */
    EXPAND_VERTICAL = (1 << 6),
    /** Don't align, expand. */
    EXPAND = EXPAND_HORIZONTAL | EXPAND_VERTICAL,
};

ENABLE_BITMASK_OPERATORS(alignmask)

/**
 * Generic orientation flags.
 */
enum class orientation
{
    HORIZONTAL,
    VERTICAL,
};

}
}

#endif
