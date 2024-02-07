/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_EVENT_H
#define EGT_EVENT_H

/**
 * @file
 * @brief Event types.
 */

#include <egt/detail/meta.h>
#include <egt/flags.h>
#include <egt/geometry.h>
#include <egt/keycode.h>
#include <iosfwd>

namespace egt
{
inline namespace v1
{

/**
 * @defgroup events Events
 * Event related functionality.
 * @ref events
 */

/**
 * Event identifiers.
 * @ingroup events
 */
enum class EventId
{
    none = detail::bit(0),

    ///@{
    /**
     * Raw pointer event.
     *
     * These events are usually tied to physical device events.
     *
     * It's usually preferred to use the normal pointer events instead.
     */
    raw_pointer_down = detail::bit(1),
    raw_pointer_up = detail::bit(2),
    raw_pointer_move = detail::bit(3),
    ///@}

    ///@{
    /** Pointer event. */
    pointer_click = detail::bit(4),
    pointer_dblclick = detail::bit(5),
    pointer_hold = detail::bit(6),
    pointer_drag_start = detail::bit(7),
    pointer_drag = detail::bit(8),
    pointer_drag_stop = detail::bit(9),
    ///@}

    ///@{
    /** Keyboard event. */
    keyboard_down = detail::bit(10),
    keyboard_up = detail::bit(11),
    keyboard_repeat = detail::bit(12),
    ///@}
};

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const EventId& event);

/**
 * Pointer event data.
 *
 * @ingroup events
 */
struct EGT_API Pointer
{
    /**
     * Definitions for pointer buttons.
     */
    enum class Button
    {
        none,
        left,
        middle,
        right
    };

    constexpr Pointer() noexcept = default;

    /**
     * @param[in] p Display point of the event.
     * @param[in] s Slot of the event.
     */
    constexpr explicit Pointer(const DisplayPoint& p, size_t s = 0) noexcept
        : point(p),
          slot(s)
    {}

    /**
     * @param[in] p Display point of the event.
     * @param[in] b Pointer button data of the event.
     * @param[in] s Slot of the event.
     */
    constexpr Pointer(const DisplayPoint& p, Button b, size_t s = 0) noexcept
        : point(p),
          btn(b),
          slot(s)
    {}

    /**
     * @param[in] p Display point of the event.
     * @param[in] d Drag start of the event.
     * @param[in] s Slot of the event.
     */
    constexpr Pointer(const DisplayPoint& p,
                      const DisplayPoint& d,
                      size_t s = 0) noexcept
        : point(p),
          drag_start(d),
          slot(s)
    {}

    /// Get the drag delta.
    EGT_NODISCARD DisplayPoint delta() const
    {
        return point - drag_start;
    }

    /// Mouse position in display coordinates.
    DisplayPoint point;

    /// Pointer button value.
    Button btn{Button::none};

    /**
     * The mouse point where EventId::pointer_drag_start occurred.
     *
     * Only valid with the following events:
     *  - EventId::pointer_drag_start
     *  - EventId::pointer_drag
     *  - EventId::pointer_drag_stop
     */
    DisplayPoint drag_start;

    /// The event slot.  Used for multi-touch.
    size_t slot{};
};

static_assert(detail::rule_of_5<Pointer>(), "must fulfill rule of 5");

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const Pointer::Button& btn);
/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const Pointer& pointer);

/**
 * Keyboard event data.
 *
 * @ingroup events
 */
struct EGT_API Key
{
    enum class KeyMod : uint32_t
    {
        shift = detail::bit(0),
        control = detail::bit(1),
        lock = detail::bit(2),
    };

    using KeyState = egt::Flags<KeyMod>;

    constexpr Key() noexcept = default;

    /**
     * @param[in] k Key code for the event.
     * @param[in] u Unicode value of the event.
     */
    constexpr explicit Key(KeyboardCode k, uint32_t u = 0, const KeyState& s = {}) noexcept
        : keycode(k),
          unicode(u),
          state(s)
    {}

    /**
     * Key code value.
     *
     * This is basically the physical key that was pressed.
     *
     * Any idea of a scancode or keysym is handled by the input backends
     * themselves, which truly represent physical keys and are used for
     * mappings.
     *
     * @see KeyboardCode
     */
    KeyboardCode keycode{EKEY_UNKNOWN};

    /**
     * 32 bit unicode code point.
     */
    uint32_t unicode{0};

    /**
     * Key state
     *
     * A bitmask of key modifiers such as SHIFT, CONTROL or LOCK keys.
     */
    KeyState state{};
};

static_assert(detail::rule_of_5<Key>(), "must fulfill rule of 5");

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const Key& key);

/**
 * Base event argument class.
 */
class EventArg
{
public:

    /**
     * Stop the event from propagating.
     */
    void stop()
    {
        m_stop = true;
    }

    /**
     * Was the event stopped from propagating?
     */
    EGT_NODISCARD bool quit() const
    {
        return m_stop;
    }

protected:

    /// Is the event stopped.
    bool m_stop{false};
};

class Widget;

/**
 * A single event that has information about the event and state for the event.
 *
 * @ingroup events
 */
struct EGT_API Event : public EventArg
{
    constexpr Event() noexcept = default;

    /**
     * @param[in] id Event id.
     */
    constexpr explicit Event(EventId id) noexcept
        : m_id(id)
    {}

    /**
     * @param[in] id Event id.
     * @param[in] pointer Pointer data for the event.
     */
    constexpr Event(EventId id, const Pointer& pointer) noexcept
        : m_id(id),
          m_pointer(pointer)
    {}

    /**
     * @param[in] id Event id.
     * @param[in] key Key data for the event.
     */
    constexpr Event(EventId id, const Key& key) noexcept
        : m_id(id),
          m_key(key)
    {}

    /// Get the id of the event.
    EGT_NODISCARD const EventId& id() const noexcept
    {
        return m_id;
    }

    /// Change the id of the event.
    void id(EventId id)
    {
        m_id = id;
    }

    /**
     * Get the Pointer event data.
     *
     * Only valid with the following events:
     *   - EventId::raw_pointer_down
     *   - EventId::raw_pointer_up
     *   - EventId::raw_pointer_move
     *   - EventId::pointer_click
     *   - EventId::pointer_dblclick
     *   - EventId::pointer_hold
     *   - EventId::pointer_drag_start
     *   - EventId::pointer_drag
     *   - EventId::pointer_drag_stop
     */
    Pointer& pointer()
    {
        return m_pointer;
    }

    /**
     * @overload
     */
    EGT_NODISCARD const Pointer& pointer() const
    {
        return m_pointer;
    }

    /**
     * Get the Key event data.
     *
     * Only valid with the following events:
     *   - EventId::keyboard_down,
     *   - EventId::keyboard_up,
     *   - EventId::keyboard_repeat,
     */
    Key& key()
    {
        return m_key;
    }

    /**
     * @overload
     */
    EGT_NODISCARD const Key& key() const
    {
        return m_key;
    };

    /**
     * Grab any related following events to this one.
     *
     * @note grab() implies stop().
     */
    void grab(Widget* widget);

    /**
     * Stop the event from propagating but only after the current handle()
     * completes.
     */
    void postpone_stop()
    {
        m_postponed_stop = true;
    }

    /**
     * Was the event scheduled to be stopped just after the latest handle()
     * completes?
     */
    EGT_NODISCARD bool postponed_quit() const
    {
        return m_postponed_stop;
    }

protected:

    /**
     * The EventId.
     */
    EventId m_id{EventId::none};

    /**
     * Key event data.
     */
    Key m_key;

    /**
     * Pointer event data.
     */
    Pointer m_pointer;

    /**
     * Stop has be rescheduled for after handle() completes.
     */
    bool m_postponed_stop{false};
};

static_assert(detail::rule_of_5<Event>(), "must fulfill rule of 5");

/// Overloaded std::ostream insertion operator
EGT_API std::ostream& operator<<(std::ostream& os, const Event& event);

}
}

#endif
