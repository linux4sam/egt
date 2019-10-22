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
#include <egt/detail/meta.h>
#include <iosfwd>
#include <memory>

namespace egt
{
inline namespace v1
{

/**
 * Alignment flags.
 */
enum class alignmask : uint32_t
{
    /** No alignment. */
    none = 0,
    /**
     * Center alignment is a weak alignment both horizontal and
     * vertical. To break one of those dimensions to another
     * alignment, specify it in addition to center.  If both
     * are broken, center has no effect.
     */
    center_horizontal = (1 << 0),
    center_vertical = (1 << 1),
    center = center_horizontal | center_vertical,
    /** Horizontal alignment. */
    left = (1 << 2),
    /** Horizontal alignment. */
    right = (1 << 3),
    /** Vertical alignment. */
    top = (1 << 4),
    /** Vertical alignment. */
    bottom = (1 << 5),
    /** Expand only horizontally. */
    expand_horizontal = (1 << 6),
    /** Expand only vertically. */
    expand_vertical = (1 << 7),
    /** Expand vertically and horizontally. */
    expand = expand_horizontal | expand_vertical,
};

ENABLE_BITMASK_OPERATORS(alignmask);

EGT_API std::ostream& operator<<(std::ostream& os, const alignmask& align);

/** Helper to set alignment of a widget. */
template<class T>
inline T& center(T& widget)
{
    widget.align(widget.align() | alignmask::center);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& center(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::center);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& center(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::center);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& left(T& widget)
{
    widget.align(widget.align() | alignmask::left);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& left(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::left);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& left(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::left);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& right(T& widget)
{
    widget.align(widget.align() | alignmask::right);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& right(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::right);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& right(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::right);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& top(T& widget)
{
    widget.align(widget.align() | alignmask::top);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& top(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::top);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& top(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::top);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& bottom(T& widget)
{
    widget.align(widget.align() | alignmask::bottom);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& bottom(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::bottom);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& bottom(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::bottom);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& expand_horizontal(T& widget)
{
    widget.align(widget.align() | alignmask::expand_horizontal);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& expand_horizontal(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::expand_horizontal);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& expand_horizontal(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::expand_horizontal);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& expand_vertical(T& widget)
{
    widget.align(widget.align() | alignmask::expand_vertical);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& expand_vertical(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::expand_vertical);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& expand_vertical(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::expand_vertical);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& expand(T& widget)
{
    widget.align(widget.align() | alignmask::expand);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& expand(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::expand);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& expand(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | alignmask::expand);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& align(T& widget, alignmask a)
{
    widget.align(a);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& align(const std::shared_ptr<T>& widget, alignmask a)
{
    widget->align(a);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& align(std::shared_ptr<T>& widget, alignmask a)
{
    widget->align(a);
    return widget;
}

/**
 * Generic orientation flags.
 */
enum class orientation
{
    horizontal,
    vertical,
    flex,
    none,
};

/**
 * Generic justification of children flags.
 */
enum class justification
{
    start,
    middle,
    ending,
    justify,
    none,
};

/**
 * Hint used for configuring Window backends.
 */
enum class windowhint
{
    /**
     * Allow automatic detection of the window type to create.
     */
    automatic,

    /**
     * Request a software only implementation.
     */
    software,

    /**
     * Request an overlay plane.
     */
    overlay,

    /**
     * Request specifically an HEO overlay plane.
     */
    heo_overlay,

    /**
     * Request a cursor overlay plane.
     */
    cursor_overlay,
};

EGT_API std::ostream& operator<<(std::ostream& os, const windowhint& event);

}
}

#endif
