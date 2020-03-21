/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_FLAGS_H
#define EGT_DETAIL_FLAGS_H

#include <egt/detail/enum.h>
#include <egt/detail/flagsbase.h>
#include <egt/detail/meta.h>
#include <egt/detail/object.h>
#include <egt/detail/signal.h>
#include <egt/detail/string.h>
#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>

/**
 * @file
 * @brief Flags handling.
 */

namespace egt
{
inline namespace v1
{
namespace detail
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
class Flags : public detail::Object, public detail::FlagsBase<T>
{
public:
    using detail::FlagsBase<T>::FlagsBase;

    constexpr Flags() noexcept = default;

    /**
     * Event signal.
     * @{
     */
    /// Invoked when the flags are changed.
    detail::Signal<> on_change;
    /** @} */

    /**
     * @param[in] str String representation of flags.
     */
    explicit Flags(const std::string& str)
    {
        this->from_string(str);
    }

    constexpr Flags(const Flags& rhs)
        : detail::Object(rhs),
          detail::FlagsBase<T>(rhs)
    {
    }

    Flags& operator=(const Flags& rhs)
    {
        if (&rhs != this)
        {
            const auto res = (*this != rhs);

            detail::Object::operator=(rhs);
            detail::FlagsBase<T>::operator=(rhs);

            if (res)
                on_change.invoke();
        }
        return *this;
    }

    bool set(const T flag) noexcept
    {
        const auto res = detail::FlagsBase<T>::set(flag);
        if (res)
            on_change.invoke();
        return res;
    }

    bool set(std::initializer_list<T> flags) noexcept
    {
        const auto res = detail::FlagsBase<T>::set(flags);
        if (res)
            on_change.invoke();
        return res;
    }

    bool clear(const T flag) noexcept
    {
        const auto res = detail::FlagsBase<T>::clear(flag);
        if (res)
            on_change.invoke();
        return res;
    }

    bool clear() noexcept
    {
        const auto res = detail::FlagsBase<T>::clear();
        if (res)
            on_change.invoke();
        return res;
    }

    constexpr Flags<T> operator|(const T& flag) const noexcept
    {
        return {this->m_flags | static_cast<typename detail::FlagsBase<T>::Underlying>(flag)};
    }

    constexpr Flags<T> operator&(const T& flag) const noexcept
    {
        return {this->m_flags& static_cast<typename detail::FlagsBase<T>::Underlying>(flag)};
    }

    /**
     * Convert the flags to strings.
     */
    std::string to_string() const
    {
        std::ostringstream ss;
        if (!this->empty())
        {
            bool first = true;
            auto container = this->get();
            for (auto& item : container)
            {
                if (first)
                    first = false;
                else
                    ss << "|";
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
        detail::tokenize(str, '|', tokens);
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
}

#endif
