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

template<class T>
struct EnumStrings
{
    static std::map<T, char const*> data;
};

template<class T>
std::string enum_to_string(T const& e)
{
    return EnumStrings<T>::data[e];
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
