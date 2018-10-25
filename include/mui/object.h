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

#include <memory>

namespace mui
{
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

            using event_callback_t = std::function<void (int event)>;

            /**
             * Add a callback to be called when the widget receives an event.
             */
            virtual void on_event(event_callback_t handler)
            {
                m_callbacks.push_back(handler);
            }

            virtual ~Object()
            {}

            virtual void invoke_handlers(int event)
            {
                // Hmm, this is not really respecting the return value of the handler
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
