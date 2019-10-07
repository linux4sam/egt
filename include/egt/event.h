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
enum class eventid
{
    none,

    ///@{
    /**
     * Raw pointer event
     *
     * It's usually preferred to use the normal pointer events instead.
     */
    raw_pointer_down,
    raw_pointer_up,
    raw_pointer_move,
    ///@}

    ///@{
    /** Pointer event. */
    pointer_click,
    pointer_dblclick,
    pointer_hold,
    pointer_drag_start,
    pointer_drag,
    pointer_drag_stop,
    ///@}

    /**
     * Sent when a widget gets focus.
     */
    enter,

    /**
     * Sent when a widget loses focus.
     */
    leave,

    ///@{
    /** Keyboard event. */
    keyboard_down,
    keyboard_up,
    keyboard_repeat,
    ///@}

    /**
     * Generated when a property changes internally.
     */
    property_changed,

    /**
     * Generated when a property changes due to user input.
     */
    input_property_changed,

    /**
     * Generated when a Widget is hidden.
     */
    hide,

    /**
     * Generated when a Widget is shown.
     */
    show,

    /**
     * Generated when the widget gains focus.
     */
    on_gain_focus,

    /**
     * Generated when the widget loses focus.
     */
    on_lost_focus,

    ///@{
    /**
     * Custom widget event.
     */
    event1,
    event2,
    ///@}
};

std::ostream& operator<<(std::ostream& os, const eventid& event);

/**
 * Pointer event data.
 *
 * @ingroup events
 */
struct Pointer
{
    constexpr Pointer() noexcept = default;

    constexpr explicit Pointer(const DisplayPoint& p) noexcept
        : point(p)
    {}

    constexpr explicit Pointer(const DisplayPoint& p,
                               const DisplayPoint& d) noexcept
        : point(p),
          drag_start(d)
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
     * Definitions for pointer buttons.
     */
    enum class button
    {
        none,
        left,
        middle,
        right
    };

    /**
     * Pointer button value.
     */
    button btn{button::none};

    /**
     * The mouse point where eventid::pointer_drag_start occurred.
     *
     * Only valid with the following events:
     *  - eventid::pointer_drag_start
     *  - eventid::pointer_drag
     *  - eventid::pointer_drag_stop
     */
    DisplayPoint drag_start;
};

static_assert(detail::rule_of_5<Pointer>(),
              "Pointer : must fulfill rule of 5");

std::ostream& operator<<(std::ostream& os, const Pointer::button& btn);
std::ostream& operator<<(std::ostream& os, const Pointer& pointer);

/**
 * Keyboard event data.
 *
 * @ingroup events
 */
struct Key
{
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

std::ostream& operator<<(std::ostream& os, const Key& key);

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
struct Event : public EventArg
{
    constexpr explicit Event(eventid id = eventid::none) noexcept
        : m_id(id)
    {}

    constexpr Event(eventid id, const DisplayPoint& point) noexcept
        : m_id(id),
          m_pointer(point)
    {}

    constexpr Event(eventid id,
                    const DisplayPoint& point,
                    const DisplayPoint& drag_start) noexcept
        : m_id(id),
          m_pointer(point, drag_start)
    {}

    Event(eventid id,
          const Pointer& pointer,
          const DisplayPoint& drag_start = {}) noexcept
        : m_id(id),
          m_pointer(pointer)
    {
        m_pointer.drag_start = drag_start;
    }

    inline const eventid& id() const noexcept
    {
        return m_id;
    }

    inline void set_id(eventid id)
    {
        m_id = id;
    }

    /**
     * Get the Pointer event data.
     *
     * Only valid with the following events:
     *   - eventid::raw_pointer_down
     *   - eventid::raw_pointer_up
     *   - eventid::raw_pointer_move
     *   - eventid::pointer_click
     *   - eventid::pointer_dblclick
     *   - eventid::pointer_hold
     *   - eventid::pointer_drag_start
     *   - eventid::pointer_drag
     *   - eventid::pointer_drag_stop
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
     *   - eventid::keyboard_down,
     *   - eventid::keyboard_up,
     *   - eventid::keyboard_repeat,
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
     * The eventid.
     */
    eventid m_id{eventid::none};

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

std::ostream& operator<<(std::ostream& os, const Event& event);

}
}

#endif
