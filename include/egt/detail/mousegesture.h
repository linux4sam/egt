/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_MOUSEGESTURE_H
#define EGT_DETAIL_MOUSEGESTURE_H

#include <chrono>
#include <egt/geometry.h>
#include <egt/input.h>
#include <egt/timer.h>
#include <egt/utils.h>
#include <vector>
#include <string>

/**
 * @file
 * @brief Mouse gesture support.
 */
namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Basic class for interpreting mouse events.
 *
 * For now, this only supports single mouse click, long click, and drag events.
 * The premise behind this class is to interpret raw mouse events and turn them
 * into higher level meaning.  Because some of those events can be asynchronous,
 * all events are generated through callbacks registered with on_async_event().
 */
template<class T>
class MouseGesture
{
public:

    enum class mouse_event
    {
        none,
        start,
        /// final event
        click,
        /// final event
        long_click,
        /// final event
        drag,
        /// final event
        drag_done
    };

    std::vector<std::string> mouse_event_names =
    {
        "none",
        "start",
        "click",
        "long_click",
        "drag",
        "drag_done"
    };

    using mouse_callback_t = std::function<void(MouseGesture<T>::mouse_event event)>;

    MouseGesture(bool allow_click = true, bool allow_drag = true)
        : m_allow_click(allow_click),
          m_allow_drag(allow_drag)
    {
        m_long_click_timer.on_timeout([this]()
        {
            stop();
            invoke_handlers(mouse_event::long_click);
        });
    }

    /**
     * Register a callback function to handle the mouse events.
     */
    void on_async_event(mouse_callback_t callback)
    {
        m_callbacks.push_back(callback);
    }

    /**
     * Pass the eventid to this function to get the proper mouse response
     * based on the state of this object.
     *
     * This should be called in a widget's handle() function with every event.
     *
     * @return Returns 1 if the event should be canceled.
     */
    int handle(eventid event)
    {
        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            invoke_handlers(mouse_event::start);
            break;
        }
        case eventid::MOUSE_UP:
        {
            if (m_active)
            {
                auto dragging = m_dragging;

                stop();

                if (dragging)
                {
                    invoke_handlers(mouse_event::drag_done);
                    return 1;
                }
                else if (m_allow_click)
                {
                    invoke_handlers(mouse_event::click);
                    return 1;
                }
            }
            break;
        }
        case eventid::MOUSE_MOVE:
        {
            if (m_active)
            {
                if (!m_dragging && m_allow_drag)
                {
                    static const auto DRAG_ENABLE_DISTANCE = 10;
                    auto distance = std::abs(m_mouse_start_pos.distance_to<int>(event_mouse()));
                    if (distance >= DRAG_ENABLE_DISTANCE)
                    {
                        m_dragging = true;
                        m_long_click_timer.cancel();
                    }
                }

                if (m_dragging)
                {
                    invoke_handlers(mouse_event::drag);
                    return 1;
                }
            }

            break;
        }
        default:
            break;
        }

        return 0;
    }

    /**
     * Start.
     *
     * @param start The starting value to save.
     */
    void start(const T& start)
    {
        m_long_click_timer.start_with_duration(std::chrono::milliseconds(500));
        m_start_value = start;
        m_mouse_start_pos = event_mouse();
        m_active = true;
        m_dragging = false;

        DBG("MouseGesture: active");
    }

    inline const T& start_value() const
    {
        return m_start_value;
    }

    inline T& start_value()
    {
        return m_start_value;
    }

    inline Point mouse_start() const
    {
        return m_mouse_start_pos;
    }

    /**
     * Is active?
     */
    inline bool active() const { return m_active; }

    /**
     * Is dragging?
     */
    inline bool dragging() const { return m_dragging; }

    /**
     * Stop any active dragging state.
     */
    void stop()
    {
        m_active = false;
        m_dragging = false;
        m_long_click_timer.cancel();
    }

protected:

    /**
     * Invoke an event on each of the handlers.
     */
    virtual void invoke_handlers(mouse_event event)
    {
        DBG("MouseGesture: " << mouse_event_names[static_cast<int>(event)]);

        for (auto& callback : m_callbacks)
            callback(event);
    }

    /**
     * Currently processessing subsequent events.
     */
    bool m_active{false};

    /**
     * Currently in the dragging state.
     */
    bool m_dragging{false};

    /**
     * The starting position of the mouse.
     */
    Point m_mouse_start_pos;

    /**
     * Arbitrary starting value requested to be saved by caller.
     */
    T m_start_value{T()};

    using callback_array = std::vector<mouse_callback_t>;

    /**
     * Registered callback functions.
     */
    callback_array m_callbacks;

    /**
     * Async timer for detecting long clicks.
     */
    Timer m_long_click_timer;

    bool m_allow_click{true};
    bool m_allow_drag{true};
};

}

}
}

#endif
