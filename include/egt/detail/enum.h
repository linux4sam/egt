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

#include <algorithm>
#include <map>
#include <stdexcept>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * When using enum_to_string() and enum_from_string(), this type need to be
 * defined and specialized to include the data for enum string conversions.
 *
 * @code{.cpp}
 * template<>
 * std::map<Pointer::Button, char const*> detail::EnumStrings<Pointer::Button>::data =
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
    static const std::map<T, char const*> data;
};

template<class T>
std::string enum_to_string(T const& e)
{
    auto i = EnumStrings<T>::data.find(e);
    return i->second;
}

template<class T>
T enum_from_string(const std::string& value)
{
    static auto begin = std::begin(EnumStrings<T>::data);
    static auto end = std::end(EnumStrings<T>::data);

    const auto result = std::find_if(begin, end,
                                     [value](const std::pair<T, char const*>& i)
    {
        return i.second == value;
    });
    if (result != end)
        return result->first;

    throw std::runtime_error("invalid enum string:" + value);
}

}
}
}

#endif
