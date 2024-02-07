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

#include <egt/detail/meta.h>
#include <egt/event.h>
#include <egt/object.h>
#include <egt/signal.h>
#include <memory>

namespace egt
{
inline namespace v1
{

namespace detail
{
class MouseGesture;
}

class Widget;

/**
 * Base Input device class.
 *
 * An input device must inherit from this class and call dispatch() in order to
 * dispatch events to the rest of the framework.
 */
class EGT_API Input
{
public:

    Input();

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;
    Input(Input&&) noexcept;
    Input& operator=(Input&&) noexcept;

    /**
     * Get a reference to the global input Object.
     *
     * This allows you to connect to any Event dispatched from any Input device.
     *
     * @b Example
     * @code{.cpp}
     * // receive any EventId::keyboard_down event from any Input device
     * Input::global_input().on_event([this](Event & event)
     * {
     *     switch (event.key().keycode)
     *     {
     *         ...
     *     }
     * }, {EventId::keyboard_down});
     * @endcode
     */
    static Object& global_input()
    {
        return m_global_handler;
    }

    virtual ~Input() noexcept;

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
    static Object m_global_handler;

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
EGT_API Widget* mouse_grab();

/**
 * Set the current widget to grab the mouse.
 *
 * @param[in] widget The widget to grab all mouse events, or nullptr.
 */
EGT_API void mouse_grab(Widget* widget);

/**
 * Set the keyboard focus of a widget.
 *
 * This will result in calling EventId::on_gain_focus for the new widget and
 * EventId::on_lost_focus for any previous widget.
 */
EGT_API void keyboard_focus(Widget* widget);

/**
 * Get the current widget which has the keyboard focus, or nullptr.
 */
EGT_API Widget* keyboard_focus();

/**
 * Get the current widget which is being dragged, or nullptr.
 */
EGT_API Widget* dragged();

/**
 * Set the current widget which is being dragged.
 */
EGT_API void dragged(Widget* widget);

}

}
}

#endif
