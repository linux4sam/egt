/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_MOUSEGESTURE_H
#define EGT_DETAIL_MOUSEGESTURE_H

#include <egt/geometry.h>
#include <egt/input.h>

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
 * For now, this only supports click and drag events.
 */
template<class T>
class MouseGesture
{
public:

    enum class mouse_event
    {
        none,
        start,
        click,
        drag,
        done
    };

    /**
     * Pass the eventid to this function to get the proper mouse response
     * based on the state of this object.
     */
    mouse_event handle(eventid event)
    {
        static const auto DRAG_ENABLE_DISTANCE = 10;

        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            return mouse_event::start;
        }
        case eventid::MOUSE_UP:
        {
            if (m_active)
            {
                auto dragging = m_dragging;

                stop();

                if (dragging)
                {
                    return mouse_event::done;
                }
                else
                {
                    return mouse_event::click;
                }
            }

            break;
        }
        case eventid::MOUSE_MOVE:
        {
            if (m_active)
            {
                if (!m_dragging)
                {
                    if (std::abs(m_mouse_start_pos.distance_to<int>(event_mouse()) >= DRAG_ENABLE_DISTANCE))
                    {
                        m_dragging = true;
                    }
                }

                if (m_dragging)
                {
                    return mouse_event::drag;
                }
            }

            break;
        }
        default:
            break;
        }

        return mouse_event::none;
    }

    /**
     * Start.
     *
     * @param start The starting value to save.
     */
    void start(const T& start)
    {
        m_start_value = start;
        m_mouse_start_pos = event_mouse();
        m_active = true;
        m_dragging = false;
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

    inline bool dragging() const { return m_dragging; }

    /**
     * Stop any active dragging state.
     */
    void stop()
    {
        m_active = false;
        m_dragging = false;
    }


protected:
    bool m_active{false};
    bool m_dragging{false};
    Point m_mouse_start_pos;
    T m_start_value;
};

}

}
}

#endif
