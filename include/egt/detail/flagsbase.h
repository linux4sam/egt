/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_FLAGSBASE_H
#define EGT_DETAIL_FLAGSBASE_H

#include <initializer_list>
#include <limits>
#include <set>

/**
 * @file Flags base.
 */

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Utility class for managing a set of 32 possible flags.
 *
 * This supports at most flag values supported by the number of bits in the
 * underlying type.
 */
template <class T>
class FlagsBase
{
public:

    /**
     * This is the underlying type of the flags, which is used internally for
     * efficient bitwise operation on flags.
     */
    using underlying = typename std::underlying_type<T>::type;

    constexpr FlagsBase() noexcept = default;

    constexpr FlagsBase(const T flag) noexcept
        : m_flags(1 << static_cast<underlying>(flag))
    {}

    FlagsBase(std::initializer_list<T> flags) noexcept
    {
        for (auto& flag : flags)
            m_flags |= (1 << static_cast<underlying>(flag));
    }

    /**
     * Test if the specified flag is set.
     * @param flag The flag to test.
     */
    constexpr inline bool is_set(const T flag) const noexcept
    {
        return m_flags & (1 << static_cast<underlying>(flag));
    }

    /**
     * Test if the specified flags are set.
     * All must be set to return true.
     * @param flags The flags to test.
     */
    inline bool is_set(std::initializer_list<T> flags) const noexcept
    {
        for (auto& flag : flags)
            if (!is_set(flag))
                return false;
        return true;
    }

    /**
     * Set the specified flag.
     * @param flag The flag to set.
     * @return True if a new item was added.
     */
    inline bool set(const T flag) noexcept
    {
        auto state = is_set(flag);
        if (!state)
            m_flags |= (1 << static_cast<underlying>(flag));
        return !state;
    }

    /**
     * Set the specified flags.
     * @param flags Flags to set.
     * @return True if a new item was added.
     */
    inline bool set(std::initializer_list<T> flags) noexcept
    {
        bool inserted = false;
        for (auto& flag : flags)
        {
            if (!is_set(flag))
            {
                m_flags |= (1 << static_cast<underlying>(flag));
                inserted = true;
            }
        }

        return inserted;
    }

    /**
     * Clear, or unset, the specified flag.
     * @param flag Flag to clear.
     * @return True if the flag was deleted.
     */
    inline bool clear(const T flag) noexcept
    {
        bool changed = false;
        if (is_set(flag))
        {
            m_flags &= ~(1 << static_cast<underlying>(flag));
            changed = true;
        }

        return changed;
    }

    /**
     * Returns true if there are no flags set.
     */
    constexpr inline bool empty() const noexcept
    {
        return m_flags == 0;
    }

    /**
     * Unset all flags.
     */
    inline bool clear() noexcept
    {
        auto orig = m_flags;
        m_flags = 0;
        return orig != m_flags;
    }

    /**
     * Get a std::set of all set flags.
     */
    inline std::set<T> get() const
    {
        std::set<T> result;
        const auto bits = std::numeric_limits<underlying>::digits;
        for (auto b = 0; b < bits; b++)
        {
            if ((m_flags & (1 << b)))
                result.insert(static_cast<T>(b));
        }

        return result;
    }

    inline underlying raw() const
    {
        return m_flags;
    }

    inline underlying& raw()
    {
        return m_flags;
    }

protected:

    constexpr FlagsBase(const underlying flags) noexcept
        : m_flags(flags)
    {}

    /**
     * The flags.
     */
    underlying m_flags{};
};

}
}
}

#endif
