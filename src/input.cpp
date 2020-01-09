/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/input.h"
#include "egt/window.h"
#include <chrono>
#include <egt/detail/mousegesture.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace egt
{
inline namespace v1
{

Input::Input()
    : m_mouse(new detail::MouseGesture)
{
    m_mouse->on_async_event([this](Event & event)
    {
        dispatch(event);
    });
}

template<class Callable>
static inline bool handler_dispatch(Event& event, Event& eevent,
                                    const Callable& handler)
{
    handler(event);
    if (event.quit())
        return true;
    if (eevent.id() != EventId::none)
    {
        handler(eevent);
        if (event.quit())
            return true;
    }

    return false;
}

/**
 * @todo No mouse positions should be allowed off the screen box().  This is
 * possible with some input devices currently and we need to limit.  Be careful
 * not to drop events (like pointer up) when correcting.
 */
void Input::dispatch(Event& event)
{
    // can't support recursive calls into the same dispatch function
    // one potential solution would be to asio::post() the call to dispatch if
    // we are currently dispatching already
    assert(!m_dispatching);

    m_dispatching = true;
    detail::ScopeExit reset([this]() { m_dispatching = false; });

    if (event.id() == EventId::raw_pointer_down)
    {
        // always reset on new down event
        detail::mouse_grab(nullptr);
    }

    auto eevent = m_mouse->handle(event);

    SPDLOG_TRACE("input event: {}", event);
    if (eevent.id() != EventId::none)
    {
        SPDLOG_TRACE("input event: {}", eevent);
    }

    if (handler_dispatch(event, eevent, [](Event & event)
{
    m_global_handler.invoke_handlers(event);
    }))
    return;

    if (modal_window())
    {
        // give event to the modal window
        auto target = modal_window();
        handler_dispatch(event, eevent, [target](Event & event)
        {
            target->handle(event);
        });

        return;
    }

    switch (event.id())
    {
    case EventId::raw_pointer_down:
    case EventId::raw_pointer_up:
    case EventId::raw_pointer_move:
    case EventId::pointer_click:
    case EventId::pointer_dblclick:
    case EventId::pointer_hold:
    case EventId::pointer_drag_start:
    case EventId::pointer_drag:
    case EventId::pointer_drag_stop:
    {
        if (detail::mouse_grab())
        {
            auto target = detail::mouse_grab();
            handler_dispatch(event, eevent, [target](Event & event)
            {
                target->handle(event);
            });
            return;
        }

        break;
    }
    case EventId::keyboard_down:
    case EventId::keyboard_up:
    case EventId::keyboard_repeat:
    {
        if (detail::keyboard_focus())
        {
            auto target = detail::keyboard_focus();
            handler_dispatch(event, eevent, [target](Event & event)
            {
                target->handle(event);
            });
            return;
        }
        break;
    }
    default:
        break;
    }

    // give event to any visible window
    for (auto& w : detail::reverse_iterate(windows()))
    {
        if (!w->top_level() ||
            w->readonly() ||
            w->disabled() ||
            !w->visible())
            continue;

        switch (event.id())
        {
        case EventId::raw_pointer_down:
        case EventId::raw_pointer_up:
        case EventId::raw_pointer_move:
        case EventId::pointer_click:
        case EventId::pointer_dblclick:
        case EventId::pointer_hold:
        case EventId::pointer_drag_start:
        case EventId::pointer_drag:
        case EventId::pointer_drag_stop:
        {
            const auto pos = w->display_to_local(event.pointer().point);
            if (!Rect(w->size()).intersect(pos))
                continue;
            break;
        }
        default:
            break;
        }

        if (handler_dispatch(event, eevent, [w](Event & event)
    {
        w->handle(event);
        }))
        return;
    }
}

Input::~Input() = default;

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
        SPDLOG_DEBUG("mouse grab by {}", widget->name());
    }
    else if (mouse_grab_widget)
    {
        SPDLOG_DEBUG("mouse release by {}", mouse_grab_widget->name());
    }
    mouse_grab_widget = widget;
}

void keyboard_focus(Widget* widget)
{
    if (keyboard_focus_widget == widget)
        return;

    if (keyboard_focus_widget)
    {
        keyboard_focus_widget->on_lost_focus.invoke();
    }

    keyboard_focus_widget = widget;

    if (widget)
    {
        widget->on_gain_focus.invoke();
    }
}

Widget* keyboard_focus()
{
    return keyboard_focus_widget;
}

}
}
}
