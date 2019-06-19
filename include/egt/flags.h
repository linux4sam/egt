/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FLAGS_H
#define EGT_FLAGS_H

#include <egt/detail/object.h>
#include <set>

namespace egt
{
inline namespace v1
{

/**
 * Utility class for managing a set of flags.
 */
template <class T>
class Flags : public detail::Object
{
public:

    using flags = std::set<T>;

    constexpr explicit Flags(const flags& f = {}) noexcept
        : m_flags(f)
    {}

    explicit Flags(const T& flag) noexcept
    {
        set(flag);
    }

    /**
     * Test if the specified flag is set.
     * @param flag The flag to test.
     */
    inline bool is_set(T flag) const
    {
        return m_flags.find(flag) != m_flags.end();
    }

    /**
     * Test if the specified flags are set.
     * @param flags The flags to test.
     */
    inline bool is_set(flags flags) const
    {
        for (auto& flag : flags)
            if (!is_text_flag_set(flag))
                return false;
        return !m_flags.empty() && !flags.empty();
    }

    /**
     * Set the specified flag.
     * @param flag The flag to set.
     * @return True if a new item was added.
     */
    inline bool set(T flag)
    {
        auto p = m_flags.insert(flag);
        if (p.second)
        {
            Event event(eventid::property_changed);
            invoke_handlers(event);
        }
        return p.second;
    }

    /**
     * Set the specified flags.
     * @param flags Flags to set.
     * @return True if a new item was added.
     */
    inline bool set(flags flags)
    {
        bool inserted = false;
        for (auto& flag : flags)
            if (set(flag))
                inserted = true;
        if (inserted)
        {
            Event event(eventid::property_changed);
            invoke_handlers(event);
        }
        return inserted;
    }

    /**
     * Clear, or unset, the specified flag.
     * @param flag Flag to clear.
     * @return True if the flag was deleted.
     */
    inline bool clear(T flag)
    {
        auto inserted = m_flags.erase(flag);
        if (inserted)
        {
            Event event(eventid::property_changed);
            invoke_handlers(event);
        }
        return inserted;
    }

    /**
     * Returns true if there are no flags set.
     */
    inline bool empty() const
    {
        return m_flags.empty();
    }

    /**
     * Get the flags set.
     */
    inline const flags& get() const
    {
        return m_flags;
    }

    inline Flags<T> operator|(const T& rhs) const
    {
        flags f(m_flags);
        f.insert(rhs);
        return Flags<T>(f);
    }

    inline Flags<T> operator|(Flags<T> rhs) const
    {
        flags f(m_flags);
        f.insert(rhs.m_flags.begin(), rhs.m_flags.end());
        return Flags<T>(f);
    }

protected:

    /**
     * The flags.
     */
    flags m_flags;
};

}
}

#endif
