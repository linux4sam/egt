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
    inline namespace v1
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

        static Widget* grab = nullptr;

        Widget* mouse_grab()
        {
            return grab;
        }

        void mouse_grab(Widget* widget)
        {
            grab = widget;
        }

        static Widget* kfocus = nullptr;

        void keyboard_focus(Widget* widget)
        {
            if (kfocus == widget)
                return;

            if (kfocus)
                kfocus->on_lost_focus();

            kfocus = widget;

            if (widget)
                widget->on_gain_focus();
        }

        Widget* keyboard_focus()
        {
            return kfocus;
        }

        namespace detail
        {
            static std::chrono::time_point<std::chrono::steady_clock> mouse_down_time;

            void IInput::dispatch(eventid event)
            {
                auto now = chrono::steady_clock::now();

                DBG("event: " << event);

                if (modal_window())
                {
                    // give event to the modal window
                    modal_window()->handle(event);
                }
                else
                {
                    if (mouse_grab() && (event == eventid::MOUSE_UP ||
                                         event == eventid::MOUSE_DOWN ||
                                         event == eventid::MOUSE_MOVE ||
                                         event == eventid::BUTTON_DOWN ||
                                         event == eventid::BUTTON_UP ||
                                         event == eventid::MOUSE_DBLCLICK ||
                                         event == eventid::MOUSE_CLICK))
                    {
                        mouse_grab()->handle(event);
                    }
                    else if (keyboard_focus() && (event == eventid::KEYBOARD_DOWN ||
                                                  event == eventid::KEYBOARD_UP ||
                                                  event == eventid::KEYBOARD_REPEAT))
                    {
                        keyboard_focus()->handle(event);
                    }
                    else
                    {

                        // give event to any top level and visible windows
                        for (auto& w : windows())
                        {
                            if (!w->visible())
                                continue;

                            if (!w->top_level())
                                continue;

                            w->handle(event);
                        }
                    }
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
}
