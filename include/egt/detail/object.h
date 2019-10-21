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
#include <egt/detail/cow.h>
#include <egt/detail/flagsbase.h>
#include <egt/detail/meta.h>
#include <egt/event.h>
#include <functional>
#include <string>
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
class EGT_API Object
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
     * find timers by name or debug.
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
    using filter_t = detail::FlagsBase<eventid>;

    /**
     * Handle type.
     */
    using handle_t = uint32_t;

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
    virtual handle_t on_event(const event_callback_t& handler,
                              filter_t mask = filter_t());

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
    virtual void clear_handlers();

    /**
     * Remove an event handler.
     *
     * @param handle The handle returned from on_event().
     */
    virtual void remove_handler(handle_t handle);

    virtual ~Object() = default;

protected:

    /**
     * Counter used to generate unique handles for each callback registration.
     */
    handle_t m_handle_counter{0};

    /**
     * Manages metadata about a registered callback.
     */
    struct EGT_API CallbackMeta
    {
        CallbackMeta(const event_callback_t& c,
                     filter_t m,
                     handle_t h) noexcept
            : callback(c),
              mask(m),
              handle(h)
        {}

        event_callback_t callback;
        filter_t mask;
        handle_t handle{0};
    };

    /**
     * Helper type for an array of callbacks.
     */
    using callback_array_t = std::vector<CallbackMeta>;

    /**
     * Array of callbacks.
     */
    detail::CopyOnWriteAllocate<callback_array_t> m_callbacks;

    /**
     * A user defined name for the Object.
     */
    std::string m_name;
};

}

}
}

#endif
