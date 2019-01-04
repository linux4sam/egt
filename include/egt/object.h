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
#include <unordered_set>

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
        struct eventid_hash
        {
            std::size_t operator()(eventid const& s) const noexcept
            {
                return std::hash<int> {}(static_cast<int>(s));
            }
        };

        /**
         * Base object class with fundamental properties.
         */
        class Object
        {
        public:

            Object() noexcept
            {}

            using event_callback_t = std::function<int (eventid event)>;

            using mask_type = std::unordered_set<eventid, eventid_hash>;

            /**
             * Add a callback to be called when the widget receives an event.
             */
            virtual void on_event(event_callback_t handler, mask_type mask = mask_type())
            {
                m_callbacks.push_back({handler, mask});
            }

            /**
             * Invoke all handlers with the specified event.
             */
            virtual int invoke_handlers(eventid event)
            {
                for (auto callback : m_callbacks)
                {
                    if (callback.mask.empty() ||
                        callback.mask.find(event) != callback.mask.end())
                    {
                        auto ret = callback.callback(event);
                        if (ret)
                            return ret;
                    }
                }

                return 0;
            }

            virtual ~Object()
            {}

        protected:

            struct callback_meta
            {
                event_callback_t callback;
                mask_type mask;
            };

            using callback_array = std::vector<callback_meta>;

            callback_array m_callbacks;
        };

    }
}

#endif
