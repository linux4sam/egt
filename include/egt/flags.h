/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FLAGS_H
#define EGT_FLAGS_H

#include <egt/detail/object.h>
#include <unordered_set>

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

    /// @private
    struct flags_hash
    {
        std::size_t operator()(T const& s) const noexcept
        {
            return std::hash<int> {}(static_cast<int>(s));
        }
    };

    using flags = std::unordered_set<T, flags_hash>;

    explicit Flags(const flags& f = flags()) noexcept
        : m_flags(f)
    {}

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
            invoke_handlers(eventid::property_changed);
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
            invoke_handlers(eventid::property_changed);
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
            invoke_handlers(eventid::property_changed);
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

protected:

    /**
     * The flags.
     */
    flags m_flags;
};

template<class T>
inline Flags<T> operator+(Flags<T> lhs, const T& rhs)
{
    lhs.set(rhs);
    return lhs;
}

template<class T>
inline Flags<T> operator-(Flags<T> lhs, const T& rhs)
{
    lhs.clear(rhs);
    return lhs;
}

}
}

#endif
