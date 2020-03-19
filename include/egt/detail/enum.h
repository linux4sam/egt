/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_ENUM_H
#define EGT_DETAIL_ENUM_H

/**
 * @file
 * @brief Fun with enum <> string conversions.
 */

#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * When using enum_to_string() and enum_from_string(), this type needs to be
 * defined and specialized to include the data for enum string conversions.
 *
 * @code{.cpp}
 * template<>
 * const std::pair<Pointer::Button, char const*> detail::EnumStrings<Pointer::Button>::data[] =
 * {
 *     {Pointer::Button::none, "none"},
 *     {Pointer::Button::left, "left"},
 *     {Pointer::Button::middle, "middle"},
 *     {Pointer::Button::right, "right"},
 * };
 *
 * std::ostream& operator<<(std::ostream& os, const Pointer::Button& btn)
 * {
 *     return os << detail::enum_to_string(btn);
 * }
 * @endcode
 */
template<class T>
struct EnumStrings
{
    /// Enum string conversion map
    static const std::pair<T, char const*> data[];
};

/// Convert an enum to a string.
template<class T>
constexpr const char* enum_to_string(T const& e)
{
    for (const auto& i : EnumStrings<T>::data)
        if (i.first == e)
            return i.second;

    throw std::runtime_error("invalid enum");
}

/// Convert a string to an enum value.
template<class T>
constexpr T enum_from_string(const char* value)
{
    for (const auto& i : EnumStrings<T>::data)
        if (std::strcmp(i.second, value) == 0)
            return i.first;

    throw std::runtime_error("invalid enum string");
}

/// Convert a string to an enum value.
template<class T>
constexpr T enum_from_string(const std::string& value)
{
    for (const auto& i : EnumStrings<T>::data)
        if (i.second == value)
            return i.first;

    throw std::runtime_error("invalid enum string");
}

}
}
}

#endif
