/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_OBJECT_H
#define EGT_OBJECT_H

/**
 * @file
 * @brief Base object definition.
 */

#include <functional>
#include <cstdint>
#include <vector>

namespace egt
{
    /**
     * Event identifiers.
     */
    enum class eventid
    {
        NONE,

        // Mouse events
        MOUSE_DOWN,
        MOUSE_UP,
        MOUSE_MOVE,
        BUTTON_DOWN,
        BUTTON_UP,
        MOUSE_DBLCLICK,
        MOUSE_CLICK,

        /**
         * Sent when a widget gets focus.
         */
        ENTER,

        /**
         * Sent when a widget loses focus.
         */
        LEAVE,

        // Keyboard Events
        KEYBOARD_DOWN,
        KEYBOARD_UP,
        KEYBOARD_REPEAT,

        /**
         * Called when a property changes.
         */
        PROPERTY_CHANGED,

        HIDE,
        SHOW,
    };

    std::ostream& operator<<(std::ostream& os, const eventid& event);

    namespace detail
    {
        /**
         * Base object class with fundamental properties.
         */
        class Object
        {
        public:

            Object() noexcept
            {}

            using event_callback_t = std::function<int (eventid event)>;

            /**
             * Add a callback to be called when the widget receives an event.
             */
            virtual void on_event(event_callback_t handler)
            {
                m_callbacks.push_back(handler);
            }

            /**
             * Invoke all handlers with the specified event.
             */
            virtual int invoke_handlers(eventid event)
            {
                for (auto callback : m_callbacks)
                {
                    auto ret = callback(event);
                    if (ret)
                        return ret;
                }

                return 0;
            }

            virtual ~Object()
            {}

        protected:

            using callback_array = std::vector<event_callback_t>;

            callback_array m_callbacks;
        };

    }
}

#endif
