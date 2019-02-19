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

Input::Input()
    : m_mouse(new detail::MouseGesture)
{
    m_mouse->on_async_event(std::bind(&Input::dispatch,
                                      this, std::placeholders::_1));
}

/**
 * @todo No mouse positions should be allowed off the screen box().  This is
 * possible with some input devices currentl and we need to limit.  Be careful
 * not to drop events (like pointer up) when correcting.
 */
void Input::dispatch(eventid event)
{
    m_current_input = this;

    auto eevent = m_mouse->handle(event);

    DBG("input event: " << event);
    if (eevent != eventid::NONE)
    {
        DBG("input event: " << eevent);
        if (eevent == eventid::POINTER_DRAG_START)
            m_pointer.drag_start = m_mouse->mouse_start();
    }

    // then give it to any global input handlers
    if (m_global_handler.invoke_handlers(event))
        return;
    if (eevent != eventid::NONE)
        if (m_global_handler.invoke_handlers(eevent))
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

Input* Input::m_current_input = nullptr;
detail::Object Input::m_global_handler;

namespace detail
{
static Widget* grab = nullptr;
static Widget* kfocus = nullptr;
}

Widget* mouse_grab()
{
    return detail::grab;
}

void mouse_grab(Widget* widget)
{
    if (widget)
    {
        DBG("mouse grab by " << widget->name());
    }
    else if (detail::grab)
    {
        DBG("mouse release by " << detail::grab->name());
    }
    detail::grab = widget;
}

void keyboard_focus(Widget* widget)
{
    if (detail::kfocus == widget)
        return;

    if (detail::kfocus)
        detail::kfocus->on_lost_focus();

    detail::kfocus = widget;

    if (widget)
        widget->on_gain_focus();
}

Widget* keyboard_focus()
{
    return detail::kfocus;
}

}
}
