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

template<class T>
inline T& center(T& widget)
{
    widget.set_align(widget.align() | alignmask::center);
    return widget;
}

template<class T>
inline const std::shared_ptr<T>& center(const std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::center);
    return widget;
}

template<class T>
inline std::shared_ptr<T>& center(std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::center);
    return widget;
}

template<class T>
inline T& left(T& widget)
{
    widget.set_align(widget.align() | alignmask::left);
    return widget;
}

template<class T>
inline const std::shared_ptr<T>& left(const std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::left);
    return widget;
}

template<class T>
inline std::shared_ptr<T>& left(std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::left);
    return widget;
}

template<class T>
inline T& right(T& widget)
{
    widget.set_align(widget.align() | alignmask::right);
    return widget;
}

template<class T>
inline const std::shared_ptr<T>& right(const std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::right);
    return widget;
}

template<class T>
inline std::shared_ptr<T>& right(std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::right);
    return widget;
}

template<class T>
inline T& top(T& widget)
{
    widget.set_align(widget.align() | alignmask::top);
    return widget;
}

template<class T>
inline const std::shared_ptr<T>& top(const std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::top);
    return widget;
}

template<class T>
inline std::shared_ptr<T>& top(std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::top);
    return widget;
}

template<class T>
inline T& bottom(T& widget)
{
    widget.set_align(widget.align() | alignmask::bottom);
    return widget;
}

template<class T>
inline const std::shared_ptr<T>& bottom(const std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::bottom);
    return widget;
}

template<class T>
inline std::shared_ptr<T>& bottom(std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::bottom);
    return widget;
}

template<class T>
inline T& expand_horizontal(T& widget)
{
    widget.set_align(widget.align() | alignmask::expand_horizontal);
    return widget;
}

template<class T>
inline const std::shared_ptr<T>& expand_horizontal(const std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::expand_horizontal);
    return widget;
}

template<class T>
inline std::shared_ptr<T>& expand_horizontal(std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::expand_horizontal);
    return widget;
}

template<class T>
inline T& expand_vertical(T& widget)
{
    widget.set_align(widget.align() | alignmask::expand_vertical);
    return widget;
}

template<class T>
inline const std::shared_ptr<T>& expand_vertical(const std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::expand_vertical);
    return widget;
}

template<class T>
inline std::shared_ptr<T>& expand_vertical(std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::expand_vertical);
    return widget;
}

template<class T>
inline T& expand(T& widget)
{
    widget.set_align(widget.align() | alignmask::expand);
    return widget;
}

template<class T>
inline const std::shared_ptr<T>& expand(const std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::expand);
    return widget;
}

template<class T>
inline std::shared_ptr<T>& expand(std::shared_ptr<T>& widget)
{
    widget->set_align(widget->align() | alignmask::expand);
    return widget;
}

template<class T>
inline T& align(T& widget, alignmask a)
{
    widget.set_align(a);
    return widget;
}

template<class T>
inline const std::shared_ptr<T>& align(const std::shared_ptr<T>& widget, alignmask a)
{
    widget->set_align(a);
    return widget;
}

template<class T>
inline std::shared_ptr<T>& align(std::shared_ptr<T>& widget, alignmask a)
{
    widget->set_align(a);
    return widget;
}

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
