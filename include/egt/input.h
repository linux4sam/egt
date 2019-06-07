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

#include <array>
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
 * Base Input device class.
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

    virtual ~Input() = default;

protected:

    /**
     * Dispatch an event from this input.
     */
    virtual void dispatch(Event& event);

    /**
     * This is the single global input handler.  Anything can attach to this
     * object and receive all events unfiltered.
     *
     * This can be useful for debugging, or simply capturing any events.
     */
    static detail::Object m_global_handler;

    /**
     * The mouse gesture handler for this input.
     */
    std::unique_ptr<detail::MouseGesture> m_mouse;

    /**
     * Currently dispatching an event when true.
     */
    bool m_dispatching{false};
};

namespace detail
{

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
}

#endif
