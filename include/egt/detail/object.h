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
    const std::string& name() const { return m_name; }

    /**
     * Set the name of the Object.
     *
     * Assigns a human readable name to an Object that can then be used to
     * find timers by name or debug.
     *
     * @param[in] name Name to set for the Object.
     */
    void name(const std::string& name) { m_name = name; }

    /**
     * Event handler callback function.
     */
    using EventCallback = std::function<void (Event& event)>;

    /**
     * Event handler EventId filter.
     */
    using FilterFlags = detail::FlagsBase<EventId>;

    /**
     * Handle type.
     */
    using RegisterHandle = uint64_t;

    /**
     * Add an event handler to be called when the widget generates an event.
     *
     * Any number of handlers (callbacks) can be registered. Also, the same
     * handler function can be registered multiple times, optionally with
     * different masks.
     *
     * @param handler The callback to invoke on event.
     * @param mask An optional EventId mask specifying what events to invoke the
     *             handler with. If no mask is supplied, any EventId will cause
     *             the handler to be invoked.
     *
     * @return A handle used to identify the registration.  This can then be
     *         passed to remove_handler().
     */
    virtual RegisterHandle on_event(const EventCallback& handler,
                                    const FilterFlags& mask = {});

    /**
     * Invoke all handlers with the specified event.
     *
     * @param event The event to invoke.
     */
    virtual void invoke_handlers(Event& event);

    /**
     * Invoke all handlers with the specified EventId.
     *
     * @param event The EventId to invoke.
     */
    virtual void invoke_handlers(EventId event);

    /**
     * Clear all registered event handlers.
     */
    virtual void clear_handlers();

    /**
     * Remove an event handler.
     *
     * @param handle The handle returned from on_event().
     */
    virtual void remove_handler(RegisterHandle handle);

    virtual ~Object() = default;

protected:

    /**
     * Counter used to generate unique handles for each callback registration.
     */
    RegisterHandle m_handle_counter{0};

    /**
     * Manages metadata about a registered callback.
     */
    struct CallbackMeta
    {
        CallbackMeta(EventCallback c,
                     const FilterFlags& m,
                     RegisterHandle h) noexcept
            : callback(std::move(c)),
              mask(m),
              handle(h)
        {}

        EventCallback callback;
        FilterFlags mask;
        RegisterHandle handle{0};
    };

    /**
     * Helper type for an array of callbacks.
     */
    using CallbackArray = std::vector<CallbackMeta>;

    /**
     * Array of callbacks.
     */
    detail::CopyOnWriteAllocate<CallbackArray> m_callbacks;

    /**
     * A user defined name for the Object.
     */
    std::string m_name;
};

}

}
}

#endif
