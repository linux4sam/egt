/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FLAGS_H
#define EGT_FLAGS_H

#include <unordered_set>

namespace egt
{
inline namespace v1
{

/**
 * This is a utility class for managing a set of flags.
 */
template <class T>
class Flags
{
public:

    struct flags_hash
    {
        std::size_t operator()(T const& s) const noexcept
        {
            return std::hash<int> {}(static_cast<int>(s));
        }
    };

    using flags = std::unordered_set<T, flags_hash>;

    explicit Flags(const flags& flags = flags()) noexcept
        : m_flags(flags)
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
     */
    inline void set(T flag)
    {
        m_flags.insert(flag);
    }

    /**
     * Set the specified textbox flags.
     * @param flags Flags to set.
     */
    inline void set(flags flags)
    {
        m_flags.insert(flags.begin(), flags.end());
    }

    /**
     * Clear, or unset, the specified flag.
     * @param flag Flag to clear.
     */
    inline void clear(T flag)
    {
        m_flags.erase(m_flags.find(flag));
    }

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

}
}

#endif
