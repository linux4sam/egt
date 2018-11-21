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
#include <linux/input.h>
#include <chrono>

using namespace std;

namespace egt
{
    Point& event_mouse()
    {
        static Point pointer_abs_pos;
        return pointer_abs_pos;
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

    namespace detail
    {
        static std::chrono::time_point<std::chrono::steady_clock> mouse_down_time;

        void IInput::dispatch(eventid event)
        {
            auto now = chrono::steady_clock::now();

            DBG("event: " << event);

            // first give event to any windos
            for (auto& w : windows())
            {
                if (!w->visible())
                    continue;

                if (!w->top_level())
                    continue;

                w->handle(event);
            }

            // then give it to any global input handlers
            m_global_input.invoke_handlers(event);

            // generate fake MOUSE_CLICK event
            if (event == eventid::MOUSE_DOWN)
                mouse_down_time = now;
            else if (event == eventid::MOUSE_UP)
            {
                if (mouse_down_time.time_since_epoch().count())
                {
                    if (chrono::duration<float_t, milli>(now - mouse_down_time).count() < 1000)
                    {
                        IInput::dispatch(eventid::MOUSE_CLICK);
                    }

                    mouse_down_time = {};
                }
            }
        }
    }

    detail::Object detail::IInput::m_global_input;
}
