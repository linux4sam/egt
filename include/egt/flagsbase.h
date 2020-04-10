/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FLAGSBASE_H
#define EGT_FLAGSBASE_H

/**
 * @file
 * @brief Flags handling.
 */

#include <egt/detail/meta.h>
#include <initializer_list>
#include <limits>
#include <set>

namespace egt
{
inline namespace v1
{

/**
 * Utility class for managing a set of flags.
 *
 * This supports at most flag values supported by the number of bits in the
 * underlying enum type.
 *
 * @warning All flags must be a power of 2.
 */
template <class T>
class FlagsBase
{
public:

    /**
     * This is the underlying type of the flags, which is used internally for
     * efficient bitwise operation on flags.
     */
    using Underlying = typename std::underlying_type<T>::type;

    constexpr FlagsBase() noexcept = default;
    EGT_OPS_COPY_MOVE(FlagsBase);
    virtual ~FlagsBase() noexcept = default;

    /// Construct with a single flag.
    // cppcheck-suppress noExplicitConstructor
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    constexpr FlagsBase(const T flag) noexcept
        : m_flags(static_cast<Underlying>(flag))
    {}

    /// Construct with an initializer_list of flags.
    // cppcheck-suppress noExplicitConstructor
    constexpr FlagsBase(std::initializer_list<T> flags) noexcept
    {
        for (auto& flag : flags)
            m_flags |= static_cast<Underlying>(flag);
    }

    /**
     * Test if the specified flag is set.
     * @param flag The flag to test.
     */
    EGT_NODISCARD constexpr bool is_set(const T flag) const noexcept
    {
        return m_flags & static_cast<Underlying>(flag);
    }

    /**
     * Test if the specified flags are set.
     * All must be set to return true.
     * @param flags The flags to test.
     */
    EGT_NODISCARD constexpr bool is_set(std::initializer_list<T> flags) const noexcept
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
    constexpr bool set(const T flag) noexcept
    {
        auto state = is_set(flag);
        if (!state)
            m_flags |= static_cast<Underlying>(flag);
        return !state;
    }

    /**
     * Set the specified flags.
     * @param flags Flags to set.
     * @return True if a new item was added.
     */
    constexpr bool set(std::initializer_list<T> flags) noexcept
    {
        bool inserted = false;
        for (auto& flag : flags)
        {
            if (!is_set(flag))
            {
                m_flags |= static_cast<Underlying>(flag);
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
    constexpr bool clear(const T flag) noexcept
    {
        bool changed = false;
        if (is_set(flag))
        {
            m_flags &= ~(static_cast<Underlying>(flag));
            changed = true;
        }

        return changed;
    }

    /**
     * Returns true if there are no flags set.
     * @return true if no flags set.
     */
    EGT_NODISCARD constexpr bool empty() const noexcept
    {
        return m_flags == 0;
    }

    /**
     * Unset all flags.
     * @return true if changed.
     */
    constexpr bool clear() noexcept
    {
        const bool diff = m_flags != 0;
        m_flags = 0;
        return diff;
    }

    /// Get a std::set of all set flags.
    EGT_NODISCARD std::set<T> get() const
    {
        std::set<T> result;
        const auto bits = std::numeric_limits<Underlying>::digits;
        for (auto b = 0; b < bits; b++)
        {
            if ((m_flags & (1u << b)))
                result.insert(static_cast<T>(1u << b));
        }

        return result;
    }

    /// Get the raw underlying value.
    EGT_NODISCARD constexpr const Underlying& raw() const
    {
        return m_flags;
    }

    /// Get the raw underlying value.
    constexpr Underlying& raw()
    {
        return m_flags;
    }

protected:

    /// @private
    // cppcheck-suppress noExplicitConstructor
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr FlagsBase(const Underlying flags) noexcept
        : m_flags(flags)
    {}

    /// The flags.
    Underlying m_flags{};
};

}
}

#endif
