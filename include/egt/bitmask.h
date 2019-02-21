/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_BITMASK_H
#define EGT_BITMASK_H

#include <type_traits>

/**
 * @file
 * @brief Working with safe bitmasks.
 */

namespace egt
{
inline namespace v1
{

/**
 * @private
 */
template<typename T>
struct EnableBitMaskOperators : std::false_type
{};

#define ENABLE_BITMASK_OPERATORS(x)                     \
    template<>                                          \
    struct EnableBitMaskOperators<x> : std::true_type   \
    {};

template<typename T>
constexpr typename std::enable_if<EnableBitMaskOperators<T>::value, T>::type
operator&(T lhs, T rhs) noexcept
{
    using underlying = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<underlying>(lhs) &
                          static_cast<underlying>(rhs));
}

template<typename T>
constexpr typename std::enable_if<EnableBitMaskOperators<T>::value, T>::type
operator|(T lhs, T rhs) noexcept
{
    using underlying = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<underlying>(lhs) |
                          static_cast<underlying>(rhs));
}

template<typename T>
constexpr typename std::enable_if<EnableBitMaskOperators<T>::value, T>::type
operator^(T lhs, T rhs) noexcept
{
    using underlying = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<underlying>(lhs) ^
                          static_cast<underlying>(rhs));
}

template<typename T>
constexpr typename std::enable_if<EnableBitMaskOperators<T>::value, T>::type
operator~(T lhs) noexcept
{
    using underlying = typename std::underlying_type<T>::type;
    return static_cast<T>(~static_cast<underlying>(lhs));
}

template<typename T>
T& operator&=(T& a, T b) noexcept
{
    a = a & b;
    return a;
}

template<typename T>
T& operator|=(T& a, T b) noexcept
{
    a = a | b;
    return a;
}

template<typename T>
T& operator^=(T& a, T b) noexcept
{
    a = a ^ b;
    return a;
}
}
}

#endif
