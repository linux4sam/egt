/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_INPUT_H
#define EGT_INPUT_H

/**
 * @file
 * @brief Working with input devices.
 */

#include <egt/detail/mousegesture.h>
#include <egt/detail/object.h>
#include <egt/event.h>
#include <egt/geometry.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

class Widget;

/**
 * @brief Base Input device class.
 *
 * This is an abstract class that must be derived from in order to
 * generate input events.
 */
class Input
{
public:

    Input();

    static inline detail::Object& global_input()
    {
        return m_global_handler;
    }

    static inline Input& current()
    {
        if (m_current_input)
            return *m_current_input;

        static Input nullinput;
        return nullinput;
    }

    /**
     * Get the Pointer object for this input.
     */
    inline const Pointer& pointer() const { return m_pointer; }

    /**
     * Get the Keys object for this input.
     */
    inline const Keys& keys() const { return m_keys; }

    virtual ~Input() = default;

protected:

    /**
     * Dispatch an event from this input.
     */
    virtual void dispatch(eventid event);

    /**
     * This is the single global input handler.  Anything can attach to this
     * object and receive all events unfiltered.
     *
     * This can be useful for debugging, or simply capturing any events.
     */
    static detail::Object m_global_handler;

    /**
     * Global current event input.
     */
    static Input* m_current_input;

    /**
     * The mouse gesture handler for this input.
     */
    std::unique_ptr<detail::MouseGesture> m_mouse;

    /**
     * Pointer object for this input.
     */
    Pointer m_pointer;

    /**
     * Keys object for this input.
     */
    Keys m_keys;

    bool m_dispatching{false};
};

/**
 * Get the current widget which has the mouse grabbed, or nullptr.
 */
Widget* mouse_grab();

/**
 * Set the current widget to grab the mouse.
 *
 * @param[in] widget The widget to grab all mouse events, or nullptr.
 */
void mouse_grab(Widget* widget);

/**
 * Set the keyboard focus of a widget.
 *
 * This will result in calling eventid::on_gain_focus for the new widget and
 * eventid::on_lost_focus for any previous widget.
 */
void keyboard_focus(Widget* widget);

/**
 * Get the current widget which has the keyboard focus, or nullptr.
 */
Widget* keyboard_focus();

}
}

#endif
