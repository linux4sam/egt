/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/object.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

uint32_t Object::on_event(const event_callback_t& handler,
                          filter_type mask)
{
    if (handler)
    {
        // TODO: m_handle_counter can wrap, making the handle non-unique
        auto handle = ++m_handle_counter;
        m_callbacks->emplace_back(handler, mask, handle);
        return handle;
    }

    return 0;
}

void Object::invoke_handlers(Event& event)
{
    if (!m_callbacks)
        return;

    for (auto& callback : *m_callbacks)
    {
        if (callback.mask.empty() ||
            callback.mask.find(event.id()) != callback.mask.end())
        {
            callback.callback(event);
            if (event.quit())
                return;
        }
    }
}

void Object::invoke_handlers(eventid event)
{
    Event e(event);
    invoke_handlers(e);
}

void Object::clear_handlers()
{
    if (!m_callbacks)
        return;

    m_callbacks->clear();
}

void Object::remove_handler(uint32_t handle)
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

}
}
}
