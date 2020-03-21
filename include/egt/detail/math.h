/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_MATH_H
#define EGT_DETAIL_MATH_H

/**
 * @file
 * @brief Math utilities.
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <type_traits>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Returns the value of PI, accurate to the type used.
 */
template<typename T>
constexpr T pi()
{
    return std::acos(-T(1));
}

/**
 * Returns the value of PI/2, accurate to the type used.
 */
template<typename T>
constexpr T pi_2()
{
    return std::acos(-T(0));
}

/// Floating point safe modulus function.
template<class T>
constexpr T mmod(T a, T n)
{
    return a - std::floor(a / n) * n;
}

/// Compute the angle different between two angles.
template<class T>
constexpr T angle_diff(T angle_start, T angle_stop, bool clockwise = true)
{
    if (clockwise)
    {
        angle_stop -= angle_start;
        if (angle_stop < 0.0)
            angle_stop += 360.;
        return angle_stop;
    }

    angle_stop -= angle_start;
    return mmod(360. - angle_stop, 360.);
}

/**
 * Convert from radians to degrees.
 */
template <class T>
constexpr T to_degrees(T radians)
{
    return radians * (180.0 / pi<T>());
}

/**
 * Convert from degrees to radians.
 */
template <class T>
constexpr T to_radians(T zero, T degrees)
{
    degrees += zero;
    return degrees * (pi<T>() / 180.0);
}

/// Clamp a value between a hi and low value.
template <class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
    return assert(!(hi < lo)),
           (v < lo) ? lo : (hi < v) ? hi : v;
}

/// constexpr version of fabs/fabsf
template <class T>
constexpr T fabs(T x,
                 typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr)
{
    return x >= 0 ? x :
           x < 0 ? -x :
           throw std::runtime_error("can't make sense of floating point value");
}

/// Safe equal comparison of float values.
constexpr bool float_equal(const float f1, const float f2)
{
    constexpr auto epsilon = 1.0e-05f;
    if (fabs(f1 - f2) <= epsilon)
        return true;
    return fabs(f1 - f2) <= epsilon * std::max(fabs(f1), fabs(f2));
}

/// Safe equal comparison of double values.
constexpr bool float_equal(const double f1, const double f2)
{
    constexpr auto epsilon = 1.0e-9;
    if (fabs(f1 - f2) <= epsilon)
        return true;
    return fabs(f1 - f2) <= epsilon * std::max(fabs(f1), fabs(f2));
}

/**
 * Normalize a value, given its min and max, to a different target min and
 * max.
 *
 * @param value The value between min and max.
 * @param min The minimum value.
 * @param max The maximum value.
 * @param target_min The minimum of the range of your desired target scaling.
 * @param target_max The maximum of the range of your desired target scaling.
 */
template<class T>
constexpr T normalize(T value, T min, T max, T target_min, T target_max)
{
    assert(!float_equal(min, max));
    assert(!float_equal(target_min, target_max));
    const auto r = ((value - min) / (max - min)) *
                   (target_max - target_min) + target_min;
    assert(!std::isnan(r));
    return r;
}

/**
 * Normalize a value, given its min and max, to a different target angle min and
 * max.
 *
 * @param value The value between min and max.
 * @param min The minimum value.
 * @param max The maximum value.
 * @param angle_start The minimum of the range of your desired target angle.
 * @param angle_stop The maximum of the range of your desired target angle.
 * @param clockwise Direction is clockwise.
 */
template<class T>
constexpr T normalize_to_angle(T value, T min, T max, T angle_start, T angle_stop, bool clockwise = true)
{
    assert(!float_equal(min, max));
    assert(!float_equal(angle_start, angle_stop));
    const auto a = normalize<T>(value, min, max, 0, angle_diff(angle_start, angle_stop, clockwise));
    assert(!std::isnan(a));
    return clockwise ? angle_start + a : angle_start - a;
}

}
}
}

#endif
