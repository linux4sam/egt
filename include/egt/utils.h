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
 * Creates and stores a simple ratio value.
 */
template<class T>
struct Ratio
{
    constexpr Ratio(T value, int ratio) noexcept
        : m_value(value),
          m_ratio(ratio)
    {
    }

    constexpr operator T() const
    {
        return static_cast<double>(m_value) *
               (static_cast<double>(m_ratio) / 100.);
    }

protected:
    T m_value;
    int m_ratio;
};

}
}

#endif
