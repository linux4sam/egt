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
 * Event identifiers.
 */
enum class eventid
{
    NONE,

    //@{
    /**
     * @brief Raw pointer events.
     * It's usually preferred to use the normal pointer evemts instead.
     */
    RAW_POINTER_DOWN,
    RAW_POINTER_UP,
    RAW_POINTER_MOVE,
    //@}

    //@{
    /** @brief Pointer events. */
    POINTER_CLICK,
    POINTER_DBLCLICK,
    POINTER_HOLD,
    POINTER_DRAG_START,
    POINTER_DRAG,
    POINTER_DRAG_STOP,
    //@}

    /**
     * Sent when a widget gets focus.
     */
    ENTER,

    /**
     * Sent when a widget loses focus.
     */
    LEAVE,

    //@{
    /** @brief Keyboard event. */
    KEYBOARD_DOWN,
    KEYBOARD_UP,
    KEYBOARD_REPEAT,
    //@}

    /**
     * Called when a property changes.
     */
    PROPERTY_CHANGED,
    INPUT_PROPERTY_CHANGED,

    HIDE,
    SHOW,
};

std::ostream& operator<<(std::ostream& os, const eventid& event);

/**
 * Definitions for pointer buttons.
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
 * Current event state for pointer.
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
 * Current event state for keyboard.
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
 * Information about the current outstanding event.
 */
namespace event
{

/**
 * Get the event Pointer object.
 */
const Pointer& pointer();

/**
 * Get the event Keys object.
 */
const Keys& keys();

}

}
}

#endif
