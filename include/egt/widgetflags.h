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
#include <egt/detail/enum.h>
#include <egt/detail/flags.h>
#include <egt/detail/meta.h>
#include <iosfwd>
#include <memory>

namespace egt
{
inline namespace v1
{

/**
 * Alignment flags.
 *
 * @note Center alignment is a weak alignment both horizontal and
 * vertical. To break one of those dimensions to another
 * alignment, specify it in addition to center.  If both
 * are broken, center has no effect.
 */
enum class AlignFlag : uint32_t
{
    /**
     * Center alignment is a weak alignment both horizontal and
     * vertical. To break one of those dimensions to another
     * alignment, specify it in addition to center.  If both
     * are broken, center has no effect.
     */
    /** Center horizontal alignment. */
    center_horizontal = detail::bit(0),
    /** Center vertical alignment. */
    center_vertical = detail::bit(1),
    /** Center horizontal and vertical alignment. */
    center = center_horizontal | center_vertical,
    /** Horizontal alignment. */
    left = detail::bit(2),
    /** Horizontal alignment. */
    right = detail::bit(3),
    /** Vertical alignment. */
    top = detail::bit(4),
    /** Vertical alignment. */
    bottom = detail::bit(5),
    /** Expand only horizontally. */
    expand_horizontal = detail::bit(6),
    /** Expand only vertically. */
    expand_vertical = detail::bit(7),
    /** Expand horizontally and vertically. */
    expand = expand_horizontal | expand_vertical,
};

template<>
const std::map<AlignFlag, char const*> detail::EnumStrings<AlignFlag>::data;

/// Alignment flags.
using AlignFlags = detail::Flags<AlignFlag>;

/// AlignFlags operator
inline AlignFlags operator|(AlignFlag lhs, AlignFlag rhs)
{
    return {lhs, rhs};
}

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const AlignFlags& align);

/** Helper to set alignment of a widget. */
template<class T>
inline T& center(T& widget)
{
    widget.align(widget.align() | AlignFlag::center);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& center(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::center);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& center(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::center);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& left(T& widget)
{
    widget.align(widget.align() | AlignFlag::left);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& left(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::left);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& left(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::left);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& right(T& widget)
{
    widget.align(widget.align() | AlignFlag::right);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& right(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::right);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& right(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::right);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& top(T& widget)
{
    widget.align(widget.align() | AlignFlag::top);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& top(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::top);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& top(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::top);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& bottom(T& widget)
{
    widget.align(widget.align() | AlignFlag::bottom);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& bottom(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::bottom);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& bottom(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::bottom);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& expand_horizontal(T& widget)
{
    widget.align(widget.align() | AlignFlag::expand_horizontal);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& expand_horizontal(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::expand_horizontal);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& expand_horizontal(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::expand_horizontal);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& expand_vertical(T& widget)
{
    widget.align(widget.align() | AlignFlag::expand_vertical);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& expand_vertical(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::expand_vertical);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& expand_vertical(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::expand_vertical);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& expand(T& widget)
{
    widget.align(widget.align() | AlignFlag::expand);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& expand(const std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::expand);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& expand(std::shared_ptr<T>& widget)
{
    widget->align(widget->align() | AlignFlag::expand);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline T& align(T& widget, AlignFlags a)
{
    widget.align(a);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline const std::shared_ptr<T>& align(const std::shared_ptr<T>& widget, AlignFlags a)
{
    widget->align(a);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
inline std::shared_ptr<T>& align(std::shared_ptr<T>& widget, AlignFlags a)
{
    widget->align(a);
    return widget;
}

/**
 * Generic orientation flags.
 */
enum class Orientation : uint32_t
{
    horizontal = detail::bit(0),
    vertical = detail::bit(1),
    flex = detail::bit(2),
    none = detail::bit(3),
};

template<>
const std::map<Orientation, char const*> detail::EnumStrings<Orientation>::data;

/**
 * Generic justification of children flags.
 */
enum class Justification : uint32_t
{
    start = detail::bit(0),
    middle = detail::bit(1),
    ending = detail::bit(2),
    justify = detail::bit(3),
    none = detail::bit(4),
};

template<>
const std::map<Justification, char const*> detail::EnumStrings<Justification>::data;

/**
 * Hint used for configuring Window backends.
 */
enum class WindowHint : uint32_t
{
    /**
     * Allow automatic detection of the window type to create.
     */
    automatic = detail::bit(0),

    /**
     * Request a software only implementation.
     */
    software = detail::bit(1),

    /**
     * Request an overlay plane.
     */
    overlay = detail::bit(2),

    /**
     * Request specifically an HEO overlay plane.
     */
    heo_overlay = detail::bit(3),

    /**
     * Request a cursor overlay plane.
     */
    cursor_overlay = detail::bit(4),
};

template<>
const std::map<WindowHint, char const*> detail::EnumStrings<WindowHint>::data;

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const WindowHint& hint);

}
}

#endif
