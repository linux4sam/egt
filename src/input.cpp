/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/input.h"
#include "egt/window.h"
#include <chrono>

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
 * possible with some input devices currently and we need to limit.  Be careful
 * not to drop events (like pointer up) when correcting.
 */
void Input::dispatch(Event& event)
{
    // can't support recursive calls into the same dispatch function
    // using the m_dispatching variable like this is not exception safe
    // one potential solution would be to asio::post() the call to dispatch if
    // we are currently dispatching already
    assert(!m_dispatching);

    m_dispatching = true;
    detail::scope_exit reset([this]() { m_dispatching = false; });

    switch (event.id())
    {
    case eventid::keyboard_down:
        m_key_states[event.key().code] = true;
        break;
    case eventid::keyboard_up:
        m_key_states[event.key().code] = false;
        break;
    case eventid::raw_pointer_down:
        // always reset on new down event
        detail::mouse_grab(nullptr);
        break;
    case eventid::raw_pointer_up:
    case eventid::raw_pointer_move:
    case eventid::pointer_click:
    case eventid::pointer_dblclick:
    case eventid::pointer_hold:
    case eventid::pointer_drag_start:
    case eventid::pointer_drag:
    case eventid::pointer_drag_stop:
    default:
        break;
    }

    auto eevent = m_mouse->handle(event);

    DBG("input event: " << event);
    if (eevent.id() != eventid::none)
    {
        DBG("input event: " << eevent);
    }

    // then give it to any global input handlers
    m_global_handler.invoke_handlers(event);
    if (event.quit())
        return;
    if (eevent.id() != eventid::none)
    {
        m_global_handler.invoke_handlers(eevent);
        if (event.quit())
            return;
    }

    if (detail::mouse_grab() && (event.id() == eventid::raw_pointer_down ||
                                 event.id() == eventid::raw_pointer_up ||
                                 event.id() == eventid::raw_pointer_move ||
                                 event.id() == eventid::pointer_click ||
                                 event.id() == eventid::pointer_dblclick ||
                                 event.id() == eventid::pointer_hold ||
                                 event.id() == eventid::pointer_drag_start ||
                                 event.id() == eventid::pointer_drag ||
                                 event.id() == eventid::pointer_drag_stop))
    {
        auto target = detail::mouse_grab();
        target->handle(event);
        if (event.quit())
            return;
        if (eevent.id() != eventid::none)
        {
            target->handle(eevent);
            if (eevent.quit())
                return;
        }
    }
    else if (detail::keyboard_focus() && (event.id() == eventid::keyboard_down ||
                                          event.id() == eventid::keyboard_up ||
                                          event.id() == eventid::keyboard_repeat))
    {
        auto target = detail::keyboard_focus();
        target->handle(event);
        if (event.quit())
            return;
        if (event.id() != eventid::none)
        {
            target->handle(eevent);
            if (eevent.quit())
                return;
        }
    }
    else if (modal_window())
    {
        auto target = modal_window();
        // give event to the modal window
        target->handle(event);
        if (event.quit())
            return;
        if (eevent.id() != eventid::none)
        {
            target->handle(eevent);
            if (eevent.quit())
                return;
        }
    }
    else
    {
        // give event to any visible window
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

            switch (event.id())
            {
            case eventid::raw_pointer_down:
            case eventid::raw_pointer_up:
            case eventid::raw_pointer_move:
            case eventid::pointer_click:
            case eventid::pointer_dblclick:
            case eventid::pointer_hold:
            case eventid::pointer_drag_start:
            case eventid::pointer_drag:
            case eventid::pointer_drag_stop:
            {
                Point pos = w->display_to_local(event.pointer().point);
                if (!Rect::point_inside(pos, w->size()))
                    continue;
                break;
            }
            default:
                break;
            }

            w->handle(event);
            if (event.quit())
                return;
            if (eevent.id() != eventid::none)
            {
                w->handle(eevent);
                if (eevent.quit())
                    return;
            }
        }
    }
}

detail::Object Input::m_global_handler;

namespace detail
{
static Widget* mouse_grab_widget = nullptr;
static Widget* keyboard_focus_widget = nullptr;

Widget* mouse_grab()
{
    return mouse_grab_widget;
}

void mouse_grab(Widget* widget)
{
    if (widget)
    {
        DBG("mouse grab by " << widget->name());
    }
    else if (mouse_grab_widget)
    {
        DBG("mouse release by " << mouse_grab_widget->name());
    }
    mouse_grab_widget = widget;
}

void keyboard_focus(Widget* widget)
{
    if (keyboard_focus_widget == widget)
        return;

    if (keyboard_focus_widget)
    {
        Event event(eventid::on_lost_focus);
        keyboard_focus_widget->handle(event);
    }

    keyboard_focus_widget = widget;

    if (widget)
    {
        Event event(eventid::on_gain_focus);
        widget->handle(event);
    }
}

Widget* keyboard_focus()
{
    return keyboard_focus_widget;
}

}

}
}
