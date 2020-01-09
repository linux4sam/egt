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

    constexpr explicit Pointer(const DisplayPoint& p, size_t s = 0) noexcept
        : point(p),
          slot(s)
    {}

    constexpr Pointer(const DisplayPoint& p, Button b, size_t s = 0) noexcept
        : point(p),
          btn(b),
          slot(s)
    {}

    constexpr Pointer(const DisplayPoint& p,
                      const DisplayPoint& d,
                      size_t s = 0) noexcept
        : point(p),
          drag_start(d),
          slot(s)
    {}

    inline DisplayPoint delta() const
    {
        return point - drag_start;
    }

    /**
     * Mouse position in display coordinates.
     */
    DisplayPoint point;

    /**
     * Pointer button value.
     */
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

    /**
     * The event slot.  Used for multitouch.
     */
    size_t slot{};
};

static_assert(detail::rule_of_5<Pointer>(),
              "Pointer : must fulfill rule of 5");

EGT_API std::ostream& operator<<(std::ostream& os, const Pointer::Button& btn);
EGT_API std::ostream& operator<<(std::ostream& os, const Pointer& pointer);

/**
 * Keyboard event data.
 *
 * @ingroup events
 */
struct EGT_API Key
{
    constexpr Key() noexcept = default;

    constexpr explicit Key(KeyboardCode k, uint32_t u = 0) noexcept
        : keycode(k),
          unicode(u)
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
};

static_assert(detail::rule_of_5<Key>(),
              "Key : must fulfill rule of 5");

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
    inline void stop()
    {
        m_stop = true;
    }

    /**
     * Was the event stopped from propagating?
     */
    inline bool quit() const
    {
        return m_stop;
    }

protected:

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
    constexpr explicit Event(EventId id = EventId::none) noexcept
        : m_id(id)
    {}

    constexpr Event(EventId id, const Pointer& pointer) noexcept
        : m_id(id),
          m_pointer(pointer)
    {}

    constexpr Event(EventId id, const Key& key) noexcept
        : m_id(id),
          m_key(key)
    {}

    inline const EventId& id() const noexcept
    {
        return m_id;
    }

    inline void id(EventId id)
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
    inline Pointer& pointer()
    {
        return m_pointer;
    }

    /**
     * @overload
     */
    inline const Pointer& pointer() const
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
    inline Key& key()
    {
        return m_key;
    }

    /**
     * @overload
     */
    inline const Key& key() const
    {
        return m_key;
    };

    /**
     * Grab any related following events to this one.
     *
     * @note grab() implies stop().
     */
    void grab(Widget* widget);

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
};

static_assert(detail::rule_of_5<Event>(),
              "Event : must fulfill rule of 5");

EGT_API std::ostream& operator<<(std::ostream& os, const Event& event);

}
}

#endif
