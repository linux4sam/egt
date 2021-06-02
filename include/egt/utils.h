/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_UTILS_H
#define EGT_UTILS_H

/**
 * @file
 * @brief Working with basic utilities.
 */

#include <egt/detail/meta.h>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * Get the version of the EGT library.
 */
EGT_API std::string egt_version();

/**
 * Get the git version of the EGT library.
 */
EGT_API std::string egt_git_version();

/**
 * Creates and stores a simple ratio value.
 */
template<class T>
struct Ratio
{
    /**
     * @param[in] value The value to base the ration off of.
     * @param[in] ratio Ratio of value to compute.
     */
    constexpr Ratio(T value, int ratio) noexcept
        : m_value(value),
          m_ratio(ratio)
    {
    }

    /**
     * Get the calculated ratio value.
     */
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr operator T() const
    {
        return static_cast<double>(m_value) *
               (static_cast<double>(m_ratio) / 100.);
    }

protected:
    /// Value
    T m_value;
    /// Ratio
    int m_ratio;
};


namespace detail
{
/// Return the size of an array (c++17's std::size()).
template <class T, std::size_t N>
constexpr std::size_t size(const T(&)[N]) noexcept
{
    return N;
}
}

}
}

#endif
