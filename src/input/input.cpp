/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mui/input.h"
#include "mui/window.h"
#include <linux/input.h>

using namespace std;

namespace mui
{

    static Point pointer_abs_pos;
    Point& mouse_position()
    {
        return pointer_abs_pos;
    }

    static int event_key_value{0};
    int& key_value()
    {
        return event_key_value;
    }

    static int event_key_code{0};
    int& key_code()
    {
        return event_key_code;
    }

    static int event_button{0};
    int& button_value()
    {
        return event_button;
    }

    void IInput::dispatch(int event)
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

    detail::Object IInput::m_global_input;
}
