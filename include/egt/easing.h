/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_EASING_H
#define EGT_EASING_H

/**
 * @file
 * @brief Animation easing functions.
 */

namespace egt
{
inline namespace v1
{
using float_t = float;

/**
 * @defgroup easing_functions Animation Easing Functions
 * These functions and functors define easing curves for use with animations.
 * @see @subpage animations
 */

/**
 * @{
 * @ingroup easing_functions
 * Predefined animation easing functions.
 */
float_t easing_linear(float_t p);
float_t easing_easy(float_t p);
float_t easing_easy_slow(float_t p);
float_t easing_extend(float_t p);
float_t easing_drop(float_t p);
float_t easing_drop_slow(float_t p);
float_t easing_snap(float_t p);
float_t easing_bounce(float_t p);
float_t easing_bouncy(float_t p);
float_t easing_rubber(float_t p);
float_t easing_spring(float_t p);
float_t easing_boing(float_t p);

float_t easing_quadratic_easein(float_t p);
float_t easing_quadratic_easeout(float_t p);
float_t easing_quadratic_easeinout(float_t p);

float_t easing_cubic_easein(float_t p);
float_t easing_cubic_easeout(float_t p);
float_t easing_cubic_easeinout(float_t p);

float_t easing_quartic_easein(float_t p);
float_t easing_quartic_easeout(float_t p);
float_t easing_quartic_easeinout(float_t p);

float_t easing_quintic_easein(float_t p);
float_t easing_quintic_easeout(float_t p);
float_t easing_quintic_easeinout(float_t p);

float_t easing_sine_easein(float_t p);
float_t easing_sine_easeout(float_t p);
float_t easing_sine_easeinout(float_t p);

float_t easing_circular_easein(float_t p);
float_t easing_circular_easeout(float_t p);
float_t easing_circular_easeinout(float_t p);

float_t easing_exponential_easein(float_t p);
float_t easing_exponential_easeout(float_t p);
float_t easing_exponential_easeinout(float_t p);
/** @} */

/**
 * Cubic Bezier equation easing function.
 *
 * @ingroup easing_functions
 */
struct easing_cubic_bezier
{
    explicit constexpr easing_cubic_bezier(float_t p0 = 0.42,
                                           float_t p1 = 0,
                                           float_t p2 = 0.58,
                                           float_t p3 = 1) noexcept
        : m_p0(p0),
          m_p1(p1),
          m_p2(p2),
          m_p3(p3)
    {}

    float_t operator()(float_t p);

    float_t m_p0;
    float_t m_p1;
    float_t m_p2;
    float_t m_p3;
};



}
}

#endif
