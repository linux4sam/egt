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

#include <cassert>
#include <cstdint>
#include <egt/bitfields.h>
#include <egt/detail/enum.h>
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
class HorizontalBitField;
class VerticalBitField;
class ExpandBitField;
class HVBitField;
class HExpandBitField;
class VExpandBitField;
class AlignFlag;
class AlginFlags;

class HorizontalBitField : private BitField
{
public:
    constexpr HorizontalBitField(uint32_t halign)
        : BitField(halign_mask, halign << halign_offset)
    {}

    constexpr bool operator==(const HorizontalBitField& rhs) const
    {
        return static_cast<const BitField*>(this)->operator==(rhs);
    }

private:
    constexpr static const uint32_t halign_offset = 0;
    constexpr static const uint32_t halign_mask = (0x3u << halign_offset);

    constexpr HorizontalBitField(const BitField& field)
        : BitField(field)
    {}

    friend constexpr HVBitField operator|(const HorizontalBitField& lhs, const VerticalBitField& rhs);
    friend constexpr HVBitField operator|(const VerticalBitField& lhs, const HorizontalBitField& rhs);
    friend constexpr HExpandBitField operator|(const HorizontalBitField& lhs, const ExpandBitField& rhs);
    friend constexpr HExpandBitField operator|(const ExpandBitField& lhs, const HorizontalBitField& rhs);
    friend constexpr AlignFlag operator|(const HorizontalBitField& lhs, const VExpandBitField& rhs);
    friend constexpr AlignFlag operator|(const VExpandBitField& lhs, const HorizontalBitField& rhs);
    friend class HVBitField;        /* HVBitField::HVBitField(uint32_t, uint32_t); */
    friend class AlignFlag;         /* AlignFlag::AlignFlag(const HorizontalBitField&) */
    friend class AlignFlags;        /* AlignFlags::AlignFlags(const HorizontalBitFields&) */
};

class VerticalBitField : private BitField
{
public:
    constexpr VerticalBitField(uint32_t valgin)
        : BitField(valign_mask, valgin << valign_offset)
    {}

    constexpr bool operator==(const VerticalBitField& rhs) const
    {
        return static_cast<const BitField*>(this)->operator==(rhs);
    }

private:
    constexpr static const uint32_t valign_offset = 2;
    constexpr static const uint32_t valign_mask = (0x3u << valign_offset);

    constexpr VerticalBitField(const BitField& field)
        : BitField(field)
    {}

    friend constexpr HVBitField operator|(const HorizontalBitField& lhs, const VerticalBitField& rhs);
    friend constexpr HVBitField operator|(const VerticalBitField& lhs, const HorizontalBitField& rhs);
    friend constexpr VExpandBitField operator|(const VerticalBitField& lhs, const ExpandBitField& rhs);
    friend constexpr VExpandBitField operator|(const ExpandBitField& lhs, const VerticalBitField& rhs);
    friend constexpr AlignFlag operator|(const VerticalBitField& lhs, const HExpandBitField& rhs);
    friend constexpr AlignFlag operator|(const HExpandBitField& lhs, const VerticalBitField& rhs);
    friend class HVBitField;        /* HVBitField::HVBitField(uint32_t, uint32_t) */
    friend class AlignFlag;         /* AlignFlag::AlignFlag(const VerticalBitField&) */
    friend class AlignFlags;        /* AlignFlags::AlignFlags(const VerticalBitFields&) */
};

class ExpandBitField : private BitField
{
public:
    constexpr ExpandBitField(uint32_t expand)
        : BitField(expand << expand_offset)
    {}

    constexpr bool operator==(const ExpandBitField& rhs) const
    {
        return static_cast<const BitField*>(this)->operator==(rhs);
    }

private:
    constexpr static const uint32_t expand_offset = 4;

    constexpr ExpandBitField(const BitField& field)
        : BitField(field)
    {}

    friend constexpr ExpandBitField operator|(const ExpandBitField& lhs, const ExpandBitField& rhs);
    friend constexpr HExpandBitField operator|(const HorizontalBitField& lhs, const ExpandBitField& rhs);
    friend constexpr HExpandBitField operator|(const ExpandBitField& lhs, const HorizontalBitField& rhs);
    friend constexpr HExpandBitField operator|(const ExpandBitField& lhs, const HExpandBitField& rhs);
    friend constexpr HExpandBitField operator|(const HExpandBitField& lhs, const ExpandBitField& rhs);
    friend constexpr VExpandBitField operator|(const VerticalBitField& lhs, const ExpandBitField& rhs);
    friend constexpr VExpandBitField operator|(const ExpandBitField& lhs, const VerticalBitField& rhs);
    friend constexpr VExpandBitField operator|(const ExpandBitField& lhs, const VExpandBitField& rhs);
    friend constexpr VExpandBitField operator|(const VExpandBitField& lhs, const ExpandBitField& rhs);
    friend constexpr AlignFlag operator|(const ExpandBitField& lhs, const HVBitField& rhs);
    friend constexpr AlignFlag operator|(const HVBitField& lhs, const ExpandBitField& rhs);
    friend constexpr AlignFlag operator|(const ExpandBitField& lhs, const AlignFlag& rhs);
    friend constexpr AlignFlag operator|(const AlignFlag& lhs, const ExpandBitField& rhs);
    friend class AlignFlag;         /* AlignFlag::AlignFlag(const ExpandBitField&) */
    friend class AlignFlags;        /* AlignFlags::AlignFlags(const ExpandBitFields&) */
};

class HVBitField : private BitField
{
public:
    constexpr HVBitField(uint32_t halign, uint32_t valign)
        : BitField(HorizontalBitField::halign_mask | VerticalBitField::valign_mask,
                   (halign << HorizontalBitField::halign_offset) | (valign << VerticalBitField::valign_offset))
    {}

    constexpr bool operator==(const HVBitField& rhs) const
    {
        return static_cast<const BitField*>(this)->operator==(rhs);
    }

private:
    constexpr HVBitField(const BitField& field)
        : BitField(field)
    {}

    friend constexpr HVBitField operator|(const HorizontalBitField& lhs, const VerticalBitField& rhs);
    friend constexpr HVBitField operator|(const VerticalBitField& lhs, const HorizontalBitField& rhs);
    friend constexpr AlignFlag operator|(const ExpandBitField& lhs, const HVBitField& rhs);
    friend constexpr AlignFlag operator|(const HVBitField& lhs, const ExpandBitField& rhs);
    friend class AlignFlag;         /* AlignFlag::AlignFlag(const HVBitField&) */
    friend class AlignFlags;        /* AlignFlags::AlignFlags(const HVBitFields&) */
};

class HExpandBitField: private BitField
{
private:
    constexpr HExpandBitField(const BitField& field)
        : BitField(field)
    {}

    friend constexpr HExpandBitField operator|(const HorizontalBitField& lhs, const ExpandBitField& rhs);
    friend constexpr HExpandBitField operator|(const ExpandBitField& lhs, const HorizontalBitField& rhs);
    friend constexpr HExpandBitField operator|(const ExpandBitField& lhs, const HExpandBitField& rhs);
    friend constexpr HExpandBitField operator|(const HExpandBitField& lhs, const ExpandBitField& rhs);
    friend constexpr AlignFlag operator|(const VerticalBitField& lhs, const HExpandBitField& rhs);
    friend constexpr AlignFlag operator|(const HExpandBitField& lhs, const VerticalBitField& rhs);
    friend class AlignFlag;         /* AlignFlag::AlignFlag(const HExpandBitField&) */
    friend class AlignFlags;        /* AlignFlags::AlignFlags(const HExpandBitFields&) */
};

class VExpandBitField : private BitField
{
private:
    constexpr VExpandBitField(const BitField& field)
        : BitField(field)
    {}

    friend constexpr VExpandBitField operator|(const VerticalBitField& lhs, const ExpandBitField& rhs);
    friend constexpr VExpandBitField operator|(const ExpandBitField& lhs, const VerticalBitField& rhs);
    friend constexpr VExpandBitField operator|(const ExpandBitField& lhs, const VExpandBitField& rhs);
    friend constexpr VExpandBitField operator|(const VExpandBitField& lhs, const ExpandBitField& rhs);
    friend constexpr AlignFlag operator|(const HorizontalBitField& lhs, const VExpandBitField& rhs);
    friend constexpr AlignFlag operator|(const VExpandBitField& lhs, const HorizontalBitField& rhs);
    friend class AlignFlag;         /* AlignFlag::AlignFlag(const VExpandBitField&) */
    friend class AlignFlags;        /* AlignFlags::AlignFlags(const VExpandBitFields&) */
};

constexpr ExpandBitField operator|(const ExpandBitField& lhs, const ExpandBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr HVBitField operator|(const HorizontalBitField& lhs, const VerticalBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr HVBitField operator|(const VerticalBitField& lhs, const HorizontalBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr HExpandBitField operator|(const HorizontalBitField& lhs, const ExpandBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr HExpandBitField operator|(const ExpandBitField& lhs, const HorizontalBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr HExpandBitField operator|(const ExpandBitField& lhs, const HExpandBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr HExpandBitField operator|(const HExpandBitField& lhs, const ExpandBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr VExpandBitField operator|(const VerticalBitField& lhs, const ExpandBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr VExpandBitField operator|(const ExpandBitField& lhs, const VerticalBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr VExpandBitField operator|(const ExpandBitField& lhs, const VExpandBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr VExpandBitField operator|(const VExpandBitField& lhs, const ExpandBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

class AlignFlag : private BitField
{
public:
    constexpr AlignFlag(const HorizontalBitField& field)
        : BitField(field)
    {}

    constexpr AlignFlag(const VerticalBitField& field)
        : BitField(field)
    {}

    constexpr AlignFlag(const ExpandBitField& field)
        : BitField(field)
    {}

    constexpr AlignFlag(const HVBitField& field)
        : BitField(field)
    {}

    constexpr AlignFlag(const HExpandBitField& field)
        : BitField(field)
    {}

    constexpr AlignFlag(const VExpandBitField& field)
        : BitField(field)
    {}

    constexpr bool operator==(const AlignFlag& rhs) const
    {
        return static_cast<const BitField*>(this)->operator==(rhs);
    }

    static constexpr HorizontalBitField center_horizontal = HorizontalBitField(1);
    static constexpr HorizontalBitField left = HorizontalBitField(2);
    static constexpr HorizontalBitField right = HorizontalBitField(3);

    static constexpr VerticalBitField center_vertical = VerticalBitField(1);
    static constexpr VerticalBitField top = VerticalBitField(2);
    static constexpr VerticalBitField bottom = VerticalBitField(3);

    static constexpr HVBitField center = center_horizontal | center_vertical;

    static constexpr ExpandBitField expand_horizontal = ExpandBitField(detail::bit(0));
    static constexpr ExpandBitField expand_vertical = ExpandBitField(detail::bit(1));
    static constexpr ExpandBitField expand = expand_horizontal | expand_vertical;

private:
    constexpr AlignFlag(const BitField& field)
        : BitField(field)
    {}

    friend constexpr AlignFlag operator|(const HorizontalBitField& lhs, const VExpandBitField& rhs);
    friend constexpr AlignFlag operator|(const VExpandBitField& lhs, const HorizontalBitField& rhs);
    friend constexpr AlignFlag operator|(const VerticalBitField& lhs, const HExpandBitField& rhs);
    friend constexpr AlignFlag operator|(const HExpandBitField& lhs, const VerticalBitField& rhs);
    friend constexpr AlignFlag operator|(const ExpandBitField& lhs, const HVBitField& rhs);
    friend constexpr AlignFlag operator|(const HVBitField& lhs, const ExpandBitField& rhs);
    friend constexpr AlignFlag operator|(const ExpandBitField& lhs, const AlignFlag& rhs);
    friend constexpr AlignFlag operator|(const AlignFlag& lhs, const ExpandBitField& rhs);
    friend class BitFields<AlignFlag>;
};

constexpr AlignFlag operator|(const HorizontalBitField& lhs, const VExpandBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr AlignFlag operator|(const VExpandBitField& lhs, const HorizontalBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr AlignFlag operator|(const VerticalBitField& lhs, const HExpandBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr AlignFlag operator|(const HExpandBitField& lhs, const VerticalBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr AlignFlag operator|(const ExpandBitField& lhs, const HVBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr AlignFlag operator|(const HVBitField& lhs, const ExpandBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr AlignFlag operator|(const ExpandBitField& lhs, const AlignFlag& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

constexpr AlignFlag operator|(const AlignFlag& lhs, const ExpandBitField& rhs)
{
    return static_cast<const BitField&>(lhs).operator | (rhs);
}

std::list<AlignFlag> get(const BitFields<AlignFlag>& fields);

/// Enum string conversion map
template<>
EGT_API const std::pair<HorizontalBitField, char const*> detail::EnumStrings<HorizontalBitField>::data[3];

template<>
EGT_API const std::pair<VerticalBitField, char const*> detail::EnumStrings<VerticalBitField>::data[3];

template<>
EGT_API const std::pair<ExpandBitField, char const*> detail::EnumStrings<ExpandBitField>::data[3];

template<>
EGT_API const std::pair<HVBitField, char const*> detail::EnumStrings<HVBitField>::data[1];

template<>
EGT_API const std::pair<AlignFlag, char const*> detail::EnumStrings<AlignFlag>::data[10];

/// Alignment flags.
class AlignFlags : public BitFields<AlignFlag>
{
public:
    using BitFields<AlignFlag>::BitFields;

    constexpr AlignFlags(const HorizontalBitField& field)
        : BitFields(static_cast<const AlignFlag&>(field))
    {}

    constexpr AlignFlags(const VerticalBitField& field)
        : BitFields(static_cast<const AlignFlag&>(field))
    {}

    constexpr AlignFlags(const ExpandBitField& field)
        : BitFields(static_cast<const AlignFlag&>(field))
    {}

    constexpr AlignFlags(const HVBitField& field)
        : BitFields(static_cast<const AlignFlag&>(field))
    {}

    constexpr AlignFlags(const HExpandBitField& field)
        : BitFields(static_cast<const AlignFlag&>(field))
    {}

    constexpr AlignFlags(const VExpandBitField& field)
        : BitFields(static_cast<const AlignFlag&>(field))
    {}

    constexpr AlignFlags(const BitFields<AlignFlag>& base)
        : BitFields(base)
    {}

    using BitFields<AlignFlag>::operator|;
};

/** Helper to set alignment of a widget. */
template<class T>
T& center(T& widget)
{
    widget.align(widget.align() | AlignFlag::center);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
const std::shared_ptr<T>& center(const std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::center);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
std::shared_ptr<T>& center(std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::center);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
T& left(T& widget)
{
    widget.align(widget.align() | AlignFlag::left);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
const std::shared_ptr<T>& left(const std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::left);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
std::shared_ptr<T>& left(std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::left);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
T& right(T& widget)
{
    widget.align(widget.align() | AlignFlag::right);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
const std::shared_ptr<T>& right(const std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::right);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
std::shared_ptr<T>& right(std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::right);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
T& top(T& widget)
{
    widget.align(widget.align() | AlignFlag::top);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
const std::shared_ptr<T>& top(const std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::top);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
std::shared_ptr<T>& top(std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::top);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
T& bottom(T& widget)
{
    widget.align(widget.align() | AlignFlag::bottom);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
const std::shared_ptr<T>& bottom(const std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::bottom);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
std::shared_ptr<T>& bottom(std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::bottom);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
T& expand_horizontal(T& widget)
{
    widget.align(widget.align() | AlignFlag::expand_horizontal);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
const std::shared_ptr<T>& expand_horizontal(const std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::expand_horizontal);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
std::shared_ptr<T>& expand_horizontal(std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::expand_horizontal);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
T& expand_vertical(T& widget)
{
    widget.align(widget.align() | AlignFlag::expand_vertical);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
const std::shared_ptr<T>& expand_vertical(const std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::expand_vertical);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
std::shared_ptr<T>& expand_vertical(std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::expand_vertical);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
T& expand(T& widget)
{
    widget.align(widget.align() | AlignFlag::expand);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
const std::shared_ptr<T>& expand(const std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::expand);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
std::shared_ptr<T>& expand(std::shared_ptr<T>& widget)
{
    assert(widget);
    widget->align(widget->align() | AlignFlag::expand);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
T& align(T& widget, const AlignFlags& a)
{
    widget.align(a);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
const std::shared_ptr<T>& align(const std::shared_ptr<T>& widget, const AlignFlags& a)
{
    assert(widget);
    widget->align(a);
    return widget;
}

/** Helper to set alignment of a widget. */
template<class T>
std::shared_ptr<T>& align(std::shared_ptr<T>& widget, const AlignFlags& a)
{
    assert(widget);
    widget->align(a);
    return widget;
}

/**
 * Generic orientation flags.
 */
enum class Orientation : uint32_t
{
    horizontal,
    vertical,
    flex,
    none,
};

/// Enum string conversion map
template<>
EGT_API const std::pair<Orientation, char const*> detail::EnumStrings<Orientation>::data[4];

/**
 * Generic justification of children flags.
 */
enum class Justification : uint32_t
{
    start,
    middle,
    ending,
    justify,
    none,
};

/// Enum string conversion map
template<>
EGT_API const std::pair<Justification, char const*> detail::EnumStrings<Justification>::data[5];

/**
 * Hint used for configuring Window backends.
 */
enum class WindowHint : uint32_t
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

/// Enum string conversion map
template<>
EGT_API const std::pair<WindowHint, char const*> detail::EnumStrings<WindowHint>::data[5];

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const WindowHint& hint);

}
}

#endif
