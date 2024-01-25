/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/mousegesture.h"
#include "egt/input.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

DefaultDim MouseGesture::m_drag_enable_distance = 10;

MouseGesture::MouseGesture() noexcept
{
    // setup long click timer handler
    m_long_click_timer.on_timeout([this]()
    {
        if (!m_holding)
            m_holding = true;
        Event event(EventId::pointer_hold, Pointer(mouse_start()));
        invoke_handlers(event);
    });
}

void MouseGesture::on_async_event(MouseCallback callback)
{
    m_callbacks.emplace_back(std::move(callback));
}

Event MouseGesture::handle(const Event& event)
{
    switch (event.id())
    {
    case EventId::raw_pointer_down:
    {
        start(event.pointer().point);
        break;
    }
    case EventId::raw_pointer_up:
    {
        if (m_active)
        {
            const auto dragging = m_dragging;
            const auto holding = m_holding;

            stop();

            if (dragging)
            {
                Event eevent(EventId::pointer_drag_stop, event.pointer());
                eevent.pointer().drag_start = mouse_start();
                return eevent;
            }
            else if (!holding)
                return {EventId::pointer_click, event.pointer()};
        }
        break;
    }
    case EventId::raw_pointer_move:
    {
        if (m_active)
        {
            bool dragging_started = false;
            if (!m_dragging)
            {
                auto distance = std::abs(mouse_start().distance_to(event.pointer().point));
                if (distance >= m_drag_enable_distance)
                {
                    m_dragging = true;
                    dragging_started = true;
                }
            }

            if (dragging_started)
            {
                Event eevent(EventId::pointer_drag_start, event.pointer());
                eevent.pointer().drag_start = mouse_start();
                return eevent;
            }

            if (m_dragging)
            {
                Event eevent(EventId::pointer_drag, event.pointer());
                eevent.pointer().drag_start = mouse_start();
                return eevent;
            }
        }

        break;
    }
    default:
        break;
    }

    return {};
}

void MouseGesture::start(const DisplayPoint& point)
{
    m_long_click_timer.start(std::chrono::milliseconds(500));
    m_mouse_start_pos = point;
    m_active = true;
    m_dragging = false;
}

void MouseGesture::stop()
{
    m_active = false;
    m_dragging = false;
    m_holding = false;
    m_long_click_timer.cancel();
}

void MouseGesture::invoke_handlers(Event& event)
{
    for (auto& callback : m_callbacks)
        callback(event);
}

}
}
}
