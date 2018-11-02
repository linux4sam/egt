/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef MUI_OBJECT_H
#define MUI_OBJECT_H

/**
 * @file
 * @brief Base object definition.
 */

#include <functional>
#include <cstdint>
#include <vector>

namespace mui
{
    /**
     * Event identifiers.
     */
    enum class eventid
    {
        NONE,

        MOUSE_DOWN,
        MOUSE_UP,
        MOUSE_MOVE,
        BUTTON_DOWN,
        BUTTON_UP,
        MOUSE_DBLCLICK,

        /**
         * Sent when a widget gets focus.
         */
        ENTER,

        /**
         * Sent when a widget loses focus.
         */
        LEAVE,
        KEYBOARD_DOWN,
        KEYBOARD_UP,
        KEYBOARD_REPEAT,

        /**
         * Called when a property changes.
         */
        PROPERTY_CHANGED,
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

            using event_callback_t = std::function<void (eventid event)>;

            /**
             * Add a callback to be called when the widget receives an event.
             */
            virtual void on_event(event_callback_t handler)
            {
                m_callbacks.push_back(handler);
            }

            virtual ~Object()
            {}

            /**
             * Invoke all handlers with the specified event.
             */
            virtual void invoke_handlers(eventid event)
            {
                // Hmm, this is not respecting the return value of the handler
                for (auto callback : m_callbacks)
                    callback(event);
            }

        protected:

            using callback_array = std::vector<event_callback_t>;

            callback_array m_callbacks;
        };

    }
}

#endif
