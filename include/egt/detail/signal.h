/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SIGNAL_H
#define EGT_DETAIL_SIGNAL_H

/**
 * @file
 * @brief Base signal definition.
 */

#include <cstdint>
#include <egt/detail/cow.h>
#include <egt/detail/meta.h>
#include <functional>
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
template<typename... Args>
class Signal
{
public:

    /**
     * Event handler callback function.
     */
    using EventCallback = std::function<void(Args...)>;

    /**
     * Handle type.
     */
    using RegisterHandle = uint32_t;

    /**
     * Add an event handler to be called when the widget generates an event.
     *
     * Any number of handlers (callbacks) can be registered. Also, the same
     * handler function can be registered multiple times, optionally with
     * different masks.
     *
     * @param handler The callback to invoke on event.
     * @return A handle used to identify the registration.  This can then be
     *         passed to remove_handler().
     */
    RegisterHandle on_event(const EventCallback& handler)
    {
        if (handler)
        {
            // TODO: m_handle_counter can wrap, making the handle non-unique
            auto handle = ++m_handle_counter;
            m_callbacks->emplace_back(handler, handle);
            return handle;
        }

        return 0;
    }

    /**
     * Convenience wrapper for on_event().
     */
    inline RegisterHandle operator()(const EventCallback& handler)
    {
        return on_event(handler);
    }

    /**
     * Invoke all handlers with the specified args.
     *
     * @param args Arguments for the handler.
     */
    void invoke(Args... args)
    {
        if (!m_callbacks)
            return;

        for (auto& callback : *m_callbacks)
            callback.callback(args...);
    }

    /**
     * Clear all registered event handlers.
     */
    void clear()
    {
        if (!m_callbacks)
            return;

        m_callbacks->clear();
    }

    /**
     * Remove an event handler.
     *
     * @param handle The handle returned from on_event().
     */
    void remove(RegisterHandle handle)
    {
        if (!m_callbacks)
            return;

        auto i = std::find_if(m_callbacks->begin(), m_callbacks->end(),
                              [handle](const CallbackMeta & meta)
        {
            return meta.handle == handle;
        });

        if (i != m_callbacks->end())
            m_callbacks->erase(i);
    }

protected:

    /**
     * Counter used to generate unique handles for each callback registration.
     */
    RegisterHandle m_handle_counter{0};

    /**
     * Manages metadata about a registered callback.
     * @private
     */
    struct EGT_API CallbackMeta
    {
        CallbackMeta(EventCallback c,
                     RegisterHandle h) noexcept
            : callback(std::move(c)),
              handle(h)
        {}

        EventCallback callback;
        RegisterHandle handle{0};
    };

    /// Helper type for an array of callbacks.
    using CallbackArray = std::vector<CallbackMeta>;

    /// Array of callbacks.
    detail::CopyOnWriteAllocate<CallbackArray> m_callbacks;
};

}

}
}

#endif
