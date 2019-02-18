/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/detail/mousegesture.h"
#include "egt/input.h"

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

MouseGesture::MouseGesture()
{
    // setup long click timer handler
    m_long_click_timer.on_timeout([this]()
    {
        stop();
        invoke_handlers(eventid::POINTER_HOLD);
    });
}

void MouseGesture::on_async_event(mouse_callback_t callback)
{
    m_callbacks.push_back(callback);
}

eventid MouseGesture::handle(eventid event)
{
    switch (event)
    {
    case eventid::RAW_POINTER_DOWN:
    {
        start();
        break;
    }
    case eventid::RAW_POINTER_UP:
    {
        if (m_active)
        {
            auto dragging = m_dragging;

            stop();

            if (dragging)
            {
                return eventid::POINTER_DRAG_STOP;
            }
            else
            {
                return eventid::POINTER_CLICK;
            }
        }
        break;
    }
    case eventid::RAW_POINTER_MOVE:
    {
        if (m_active)
        {
            bool dragging_started = false;
            if (!m_dragging)
            {
                static const auto DRAG_ENABLE_DISTANCE = 10;
                auto distance = std::abs(m_mouse_start_pos.distance_to<int>(event::pointer().point));
                if (distance >= DRAG_ENABLE_DISTANCE)
                {
                    m_dragging = true;
                    dragging_started = true;

                    /// @bug This may be a problem.  It is possble the long click
                    /// event will still be generated.
                    m_long_click_timer.cancel();
                }
            }

            if (dragging_started)
                return eventid::POINTER_DRAG_START;
            if (m_dragging)
                return eventid::POINTER_DRAG;
        }

        break;
    }
    default:
        break;
    }

    return eventid::NONE;
}

void MouseGesture::start()
{
    m_long_click_timer.start_with_duration(std::chrono::milliseconds(500));
    m_mouse_start_pos = event::pointer().point;
    m_active = true;
    m_dragging = false;
}

void MouseGesture::stop()
{
    m_active = false;
    m_dragging = false;
    m_long_click_timer.cancel();
}

void MouseGesture::invoke_handlers(eventid event)
{
    for (auto& callback : m_callbacks)
        callback(event);
}

}
}
}
