/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/easing.h"
#include "egt/detail/math.h"
#include <cmath>

namespace egt
{
inline namespace v1
{

constexpr static auto NEGCOSPI = float_t(1.61803398874989);

float_t easing_linear(float_t p)
{
    return p;
}

float_t easing_easy(float_t p)
{
    return p * p * (3.0f - 2.0f * p);
}

float_t easing_easy_slow(float_t p)
{
    p = easing_easy(p);
    return p * p * (3.0f - 2.0f * p);
}

float_t easing_extend(float_t p)
{
    p = (p * 1.4f) - 0.2f;
    return 0.5f - std::cos(detail::pi<float_t>() * p) / NEGCOSPI;
}

static inline float_t power(float_t base, float_t exponent)
{
    if (detail::float_compare(exponent, 0.0f))
        return 1.0f;
    else if (detail::float_compare(base, 0.0f) && exponent > 0.0f)
        return 0.0f;

    return std::exp(exponent * std::log(base));
}

float_t easing_drop(float_t p)
{
    return p * p;
}

float_t easing_drop_slow(float_t p)
{
    return p * p * p * p * p;
}

float_t easing_snap(float_t p)
{
    p = p * p;
    p = (p * 1.4f) - 0.2f;
    return 0.5f - std::cos(detail::pi<float_t>() * p) / NEGCOSPI;
}

float_t easing_bounce(float_t p)
{
    if (p > 0.9f)
    {
        float_t result = p - 0.95f;
        return 1.0f + result * result * 20.0f - (0.05f * 0.05f * 20.0f);
    }
    else if (p > 0.75f)
    {
        float_t result = p - 0.825f;
        return 1.0f + result * result * 16.0f - (0.075f * 0.075f * 16.0f);
    }
    else if (p > 0.5f)
    {
        float_t result = p - 0.625f;
        return 1.0f + result * result * 12.0f - (0.125f * 0.125f * 12.0f);
    }

    p = p * 2.0f;
    return p * p;
}

float_t easing_bouncy(float_t p)
{
    float_t result = 1.0f;
    float_t scale = 5.0f;
    float_t start = 0.5f;
    float_t step = 0.2f;

    if (p < start)
    {
        result = p / start;
        return result * result;
    }

    while (step > 0.01f)
    {
        if (p < start + step)
        {
            step = step / 2.0f;
            result = (p - (start + step)) * scale;
            result =  result * result;
            result = result + 1.0f - power(step * scale, 2.0f);
            break;
        }
        else
        {
            start = start + step;
            step = step * 0.6f;
        }
    }

    return result;
}

float_t easing_rubber(float_t p)
{
    if (p > 0.9f)
    {
        float_t result = p - 0.95f;
        return 1.0f - result * result * 20.0f + (0.05f * 0.05f * 20.0f);
    }
    else if (p > 0.75f)
    {
        float_t result = p - 0.825f;
        return 1.0f + result * result * 18.0f - (0.075f * 0.075f * 18.0f);
    }
    else if (p > 0.5f)
    {
        float_t result = p - 0.625f;
        return 1.0f - result * result * 14.0f + (0.125f * 0.125f * 14.0f);
    }

    p = p * 2.0f;
    return p * p;
}

float_t easing_spring(float_t p)
{
    p = p * p;
    float_t result = std::sin(detail::pi<float_t>() * p * p * 10.0f - detail::pi<float_t>() / 2.0f) / 4.0f;
    result = result * (1.0f - p) + 1.0f;
    if (p < 0.3f)
        result = result * easing_easy(p / 0.3f);
    return result;
}

float_t easing_boing(float_t p)
{
    p = power(p, 1.5f);
    float_t result = std::sin(detail::pi<float_t>() * power(p, 2.0f) * 20.0f - detail::pi<float_t>() / 2.0f) / 4.0f;
    result = result * (1.0f - p) + 1.0f;
    if (p < 0.2f)
        result = result * easing_easy(p / 0.2f);
    return result;
}

float_t easing_quadratic_easein(float_t p)
{
    return p * p;
}

float_t easing_quadratic_easeout(float_t p)
{
    return -(p * (p - 2));
}

float_t easing_quadratic_easeinout(float_t p)
{
    if (p < 0.5)
    {
        return 2 * p * p;
    }
    else
    {
        return (-2 * p * p) + (4 * p) - 1;
    }
}

float_t easing_cubic_easein(float_t p)
{
    return p * p * p;
}

float_t easing_cubic_easeout(float_t p)
{
    float_t f = (p - 1);
    return f * f * f + 1;
}

float_t easing_cubic_easeinout(float_t p)
{
    if (p < 0.5)
    {
        return 4 * p * p * p;
    }
    else
    {
        float_t f = ((2 * p) - 2);
        return 0.5 * f * f * f + 1;
    }
}

float_t easing_quartic_easein(float_t p)
{
    return p * p * p * p;
}

float_t easing_quartic_easeout(float_t p)
{
    float_t f = (p - 1);
    return f * f * f * (1 - p) + 1;
}

float_t easing_quartic_easeinout(float_t p)
{
    if (p < 0.5)
    {
        return 8 * p * p * p * p;
    }
    else
    {
        float_t f = (p - 1);
        return -8 * f * f * f * f + 1;
    }
}

float_t easing_quintic_easein(float_t p)
{
    return p * p * p * p * p;
}

float_t easing_quintic_easeout(float_t p)
{
    float_t f = (p - 1);
    return f * f * f * f * f + 1;
}

float_t easing_quintic_easeinout(float_t p)
{
    if (p < 0.5)
    {
        return 16 * p * p * p * p * p;
    }
    else
    {
        float_t f = ((2 * p) - 2);
        return  0.5 * f * f * f * f * f + 1;
    }
}

float_t easing_sine_easein(float_t p)
{
    return std::sin((p - 1) * detail::pi_2<float_t>()) + 1;
}

float_t easing_sine_easeout(float_t p)
{
    return std::sin(p * detail::pi_2<float_t>());
}

float_t easing_sine_easeinout(float_t p)
{
    return 0.5 * (1 - std::cos(p * detail::pi<float_t>()));
}

float_t easing_circular_easein(float_t p)
{
    return 1 - std::sqrt(1 - (p * p));
}

float_t easing_circular_easeout(float_t p)
{
    return std::sqrt((2 - p) * p);
}

float_t easing_circular_easeinout(float_t p)
{
    if (p < 0.5)
    {
        return 0.5 * (1 - std::sqrt(1 - 4 * (p * p)));
    }
    else
    {
        return 0.5 * (std::sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
    }
}

float_t easing_exponential_easein(float_t p)
{
    return detail::float_compare(p, 0.0f) ? p : std::pow(2, 10 * (p - 1));
}

float_t easing_exponential_easeout(float_t p)
{
    return detail::float_compare(p, 1.0f) ? p : 1 - std::pow(2, -10 * p);
}

float_t easing_exponential_easeinout(float_t p)
{
    if (detail::float_compare(p, 0.0f) || detail::float_compare(p, 1.0f))
        return p;

    if (p < 0.5)
    {
        return 0.5 * std::pow(2, (20 * p) - 10);
    }
    else
    {
        return -0.5 * std::pow(2, (-20 * p) + 10) + 1;
    }
}

float_t easing_cubic_bezier::operator()(float_t p)
{
    return std::pow(1. - p, 3.) * m_p0 +
           3. * p * std::pow(1. - p, 2.) * m_p1 +
           3. * std::pow(p, 2.) * (1. - p) * m_p2 +
           std::pow(p, 3.) * m_p3;
}

}
}
