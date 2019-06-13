/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_OBJECT_H
#define EGT_DETAIL_OBJECT_H

/**
 * @file
 * @brief Base object definition.
 */

#include <cstdint>
#include <egt/event.h>
#include <functional>
#include <string>
#include <set>
#include <vector>

namespace egt
{
inline namespace v1
{

namespace detail
{

/**
 * Base object class with fundamental properties.
 */
class Object
{
public:

    Object() noexcept = default;

    /**
     * Get the name of the Object.
     */
    inline const std::string& name() const { return m_name; }

    /**
     * Set the name of the Object.
     *
     * Assigns a human readable name to an Object that can then be used to
     * find Objects by name or debug Objects.
     *
     * @param[in] name Name to set for the Object.
     */
    inline void set_name(const std::string& name) { m_name = name; }

    /**
     * Event handler callback function.
     */
    using event_callback_t = std::function<void (Event& event)>;

    /**
     * Event handler eventid filter.
     */
    using filter_type = std::set<eventid>;

    /**
     * Add an event handler to be called when the widget generates an event.
     *
     * Any number of handlers (callbacks) can be registered. Also, the same
     * handler function can be registered multiple times, optionally with
     * different masks.
     *
     * @param handler The callback to invoke on event.
     * @param mask An optional eventid mask specifying what events to invoke the
     *             handler with. If no mask is supplied, any eventid will cause
     *             the handler to be invoked.
     *
     * @return A handle used to identify the registration.  This can then be
     *         passed to remove_handler().
     */
    virtual uint32_t on_event(event_callback_t handler,
                              filter_type mask = filter_type());

    /**
     * Invoke all handlers with the specified event.
     *
     * @param event The event to invoke.
     */
    virtual void invoke_handlers(Event& event);

    /**
     * Invoke all handlers with the specified eventid.
     *
     * @param event The eventid to invoke.
     */
    virtual void invoke_handlers(eventid event);

    /**
     * Clear all registered event handlers.
     */
    virtual void clear_event_handlers();

    /**
     * Remove an event handler.
     *
     * @param handle The handle returned from on_event().
     */
    virtual void remove_handler(uint32_t handle);

    virtual ~Object() = default;

protected:

    /**
     * Counter used to generate unique handles for each callback registration.
     */
    uint32_t m_handle_counter{0};

    /**
     * Manages metadata about a registered callback.
     */
    struct callback_meta
    {
        callback_meta(event_callback_t c,
                      filter_type m,
                      uint32_t h) noexcept
            : callback(std::move(c)),
              mask(std::move(m)),
              handle(h)
        {}

        event_callback_t callback;
        filter_type mask;
        uint32_t handle{0};
    };

    /**
     * Helper type for an array of callbacks.
     */
    using callback_array = std::vector<callback_meta>;

    /**
     * Array of callbacks.
     */
    callback_array m_callbacks;

    /**
     * A user defined name for the Object.
     */
    std::string m_name;
};

}

}
}

#endif
