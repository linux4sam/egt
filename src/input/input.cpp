/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/input.h"
#include "egt/window.h"
#include <linux/input.h>

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
        void IInput::dispatch(eventid event)
        {
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
        }
    }

    detail::Object detail::IInput::m_global_input;
}
