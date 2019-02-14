/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/input.h"
#include "egt/window.h"
#include <chrono>
#include <linux/input.h>

using namespace std;

namespace egt
{
inline namespace v1
{
Point& event_mouse()
{
    static Point pointer_abs_pos;
    return pointer_abs_pos;
}

Point event_mouse_drag_start()
{
    return detail::IInput::m_mouse->mouse_start();
}

int& event_key()
{
    static int event_event_key{0};
    return event_event_key;
}

int& event_code()
{
    static int event_event_code{0};
    return event_event_code;
}

int& event_button()
{
    static int event_button_value{0};
    return event_button_value;
}

static Widget* grab = nullptr;

Widget* mouse_grab()
{
    return grab;
}

void mouse_grab(Widget* widget)
{
    if (widget)
    {
        DBG("mouse grab by " << widget->name());
    }
    else if (grab)
    {
        DBG("mouse release by " << grab->name());
    }
    grab = widget;
}

static Widget* kfocus = nullptr;

void keyboard_focus(Widget* widget)
{
    if (kfocus == widget)
        return;

    if (kfocus)
        kfocus->on_lost_focus();

    kfocus = widget;

    if (widget)
        widget->on_gain_focus();
}

Widget* keyboard_focus()
{
    return kfocus;
}

namespace detail
{
detail::MouseGesture* IInput::m_mouse = nullptr;

void IInput::dispatch(eventid event)
{
    // hack
    if (!m_mouse)
    {
        m_mouse = new MouseGesture;
        m_mouse->on_async_event(IInput::dispatch);
    }

    auto eevent = m_mouse->handle(event);

    DBG("input event: " << event);
    if (eevent != eventid::NONE)
    {
        DBG("input event: " << eevent);
    }

    // then give it to any global input handlers
    if (m_global_input.invoke_handlers(event))
        return;
    if (eevent != eventid::NONE)
        if (m_global_input.invoke_handlers(eevent))
            return;


    if (modal_window())
    {
        auto target = modal_window();
        // give event to the modal window
        target->handle(event);
        if (eevent != eventid::NONE)
            target->handle(eevent);
    }
    else
    {
        if (mouse_grab() && (event == eventid::RAW_POINTER_DOWN ||
                             event == eventid::RAW_POINTER_UP ||
                             event == eventid::RAW_POINTER_MOVE ||
                             event == eventid::POINTER_CLICK ||
                             event == eventid::POINTER_DBLCLICK ||
                             event == eventid::POINTER_HOLD ||
                             event == eventid::POINTER_DRAG_START ||
                             event == eventid::POINTER_DRAG ||
                             event == eventid::POINTER_DRAG_STOP))
        {
            auto target = mouse_grab();
            target->handle(event);
            if (eevent != eventid::NONE)
                target->handle(eevent);
        }
        else if (keyboard_focus() && (event == eventid::KEYBOARD_DOWN ||
                                      event == eventid::KEYBOARD_UP ||
                                      event == eventid::KEYBOARD_REPEAT))
        {
            auto target = keyboard_focus();
            target->handle(event);
            if (event != eventid::NONE)
                target->handle(eevent);
        }
        else
        {
            // give event to any top level and visible windows
            for (auto& w : detail::reverse_iterate(windows()))
            {
                if (!w->top_level())
                    continue;

                if (w->readonly())
                    continue;

                if (w->disabled())
                    continue;

                if (!w->visible())
                    continue;

                w->handle(event);
                if (eevent != eventid::NONE)
                    w->handle(eevent);
            }
        }
    }

}
}

detail::Object detail::IInput::m_global_input;
}
}
