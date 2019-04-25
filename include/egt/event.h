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

#include <array>
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
 */

/**
 * @brief Event identifiers.
 * @ingroup events
 */
enum class eventid
{
    none,

    //@{
    /**
     * @brief Raw pointer events.
     * It's usually preferred to use the normal pointer events instead.
     */
    raw_pointer_down,
    raw_pointer_up,
    raw_pointer_move,
    //@}

    //@{
    /** @brief Pointer events. */
    pointer_click,
    pointer_dblclick,
    pointer_hold,
    pointer_drag_start,
    pointer_drag,
    pointer_drag_stop,
    //@}

    /**
     * Sent when a widget gets focus.
     */
    enter,

    /**
     * Sent when a widget loses focus.
     */
    leave,

    //@{
    /** @brief Keyboard event. */
    keyboard_down,
    keyboard_up,
    keyboard_repeat,
    //@}

    /**
     * Called when a property changes.
     */
    property_changed,
    input_property_changed,

    hide,
    show,

    /**
     * Called when the widget gains focus.
     */
    on_gain_focus,

    /**
     * Called when the widget loses focus.
     */
    on_lost_focus,

    event1,
    event2,
};

std::ostream& operator<<(std::ostream& os, const eventid& event);

/**
 * @brief Definitions for pointer buttons.
 */
enum class pointer_button
{
    none,
    left,
    middle,
    right,
    touch
};

/**
 * @brief Current event state for pointer.
 * @ingroup events
 */
struct Pointer
{
    /**
     * Mouse position in display coordinates.
     */
    DisplayPoint point;

    /**
     * Pointer button value.
     */
    pointer_button button{pointer_button::none};

    /**
     * The mouse point where eventid POINTER_DRAG_START occured.
     */
    DisplayPoint drag_start;
};

/**
 * @brief Current event state for keyboard.
 * @ingroup events
 */
struct Keys
{
    /**
     * Key value.
     */
    int key;

    /**
     * Key code.
     */
    int code;

    /**
     * Boolean state of every key code.
     */
    std::array<bool, 256> states{};
};

/**
 * @brief Information about the current outstanding event.
 */
namespace event
{

/**
 * Get the event Pointer object.
 * @ingroup events
 */
const Pointer& pointer();

/**
 * Get the event Keys object.
 * @ingroup events
 */
const Keys& keys();

}

}
}

#endif
