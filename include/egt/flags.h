/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_FLAGS_H
#define EGT_FLAGS_H

/**
 * @file
 * @brief Flags handling.
 */

#include <egt/detail/enum.h>
#include <egt/detail/meta.h>
#include <egt/detail/string.h>
#include <egt/flagsbase.h>
#include <egt/signal.h>
#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * Utility class for managing a set of flags with the ability to observe changes
 * to the flags.
 *
 * This supports at most flag values supported by the number of bits in the
 * underlying enum type.
 *
 * @warning All flags must be a power of 2.
 */
template<class T>
class Flags : public FlagsBase<T>
{
public:
    using FlagsBase<T>::FlagsBase;

    EGT_OPS_MOVE(Flags);
    ~Flags() noexcept override = default;

    /**
     * Event signal.
     * @{
     */
    /// Invoked when the flags are changed.
    Signal<> on_change;
    /** @} */

    /**
     * @param[in] str String representation of flags.
     */
    explicit Flags(const std::string& str)
    {
        this->from_string(str);
    }

    /**
     * @param[in] str String representation of flags.
     */
    explicit Flags(const char* str)
    {
        this->from_string(str);
    }

    /// Set a single flag.
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr Flags(T flag) noexcept
        : FlagsBase<T>(flag)
    {
    }

    /// Copy constructor.
    constexpr Flags(const Flags& rhs) noexcept
        : FlagsBase<T>(rhs)
    {
        // signals are not copied!
    }

    /// Assignment operator.
    // NOLINTNEXTLINE(cert-oop54-cpp)
    Flags& operator=(const Flags& rhs)
    {
        if (&rhs != this)
        {
            const auto res = (*this != rhs);

            // signals are not copied!
            FlagsBase<T>::operator=(rhs);

            if (res)
                on_change.invoke();
        }
        return *this;
    }

    /// Set a single flag.
    bool set(T flag) noexcept
    {
        const auto res = FlagsBase<T>::set(flag);
        if (res)
            on_change.invoke();
        return res;
    }

    /// Set multiple flags.
    bool set(std::initializer_list<T> flags) noexcept
    {
        const auto res = FlagsBase<T>::set(flags);
        if (res)
            on_change.invoke();
        return res;
    }

    /// Clear a single flag.
    bool clear(T flag) noexcept
    {
        const auto res = FlagsBase<T>::clear(flag);
        if (res)
            on_change.invoke();
        return res;
    }

    /// Clear all flags.
    bool clear() noexcept
    {
        const auto res = FlagsBase<T>::clear();
        if (res)
            on_change.invoke();
        return res;
    }

    /// Or operator.
    constexpr Flags<T> operator|(const T& flag) const noexcept
    {
        return {this->m_flags | static_cast<typename FlagsBase<T>::Underlying>(flag)};
    }

    /// And operator.
    constexpr Flags<T> operator&(const T& flag) const noexcept
    {
        return {this->m_flags& static_cast<typename FlagsBase<T>::Underlying>(flag)};
    }

    /// Delimiter used to separate flags in string representation.
    constexpr static const char FLAGS_DELIMITER = '|';

    /**
     * Convert the flags to strings.
     */
    EGT_NODISCARD std::string to_string() const
    {
        std::ostringstream ss;
        if (!this->empty())
        {
            bool first = true;
            const auto container = this->get();
            for (const auto& item : container)
            {
                if (first)
                    first = false;
                else
                    ss << FLAGS_DELIMITER;
                ss << detail::enum_to_string(item);
            }
        }
        return ss.str();
    }

    /**
     * Convert from string.
     *
     * @note This will clear any existing flags first.
     */
    void from_string(const std::string& str)
    {
        this->clear();
        std::vector<std::string> tokens;
        detail::tokenize(str, FLAGS_DELIMITER, tokens);
        for (auto& flag : tokens)
            this->set(detail::enum_from_string<T>(detail::trim(flag)));
    }
};

/// @private
enum class RuleE {};
static_assert(detail::rule_of_5<Flags<RuleE>>(), "must fulfill rule of 5");

/// Flags operator
template<class T>
constexpr bool operator==(const Flags<T>& lhs, const Flags<T>& rhs)
{
    return lhs.raw() == rhs.raw();
}

/// Flags operator
template<class T>
constexpr bool operator!=(const Flags<T>& lhs, const Flags<T>& rhs)
{
    return !(lhs == rhs);
}

/// Overloaded std::ostream insertion operator
template<class T>
std::ostream& operator<<(std::ostream& os, const Flags<T>& flags)
{
    return os << flags.tostring();
}

}
}

#endif
