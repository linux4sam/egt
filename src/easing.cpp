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

constexpr static auto NEGCOSPI = EasingScalar(1.61803398874989);

EasingScalar easing_linear(EasingScalar p)
{
    return p;
}

EasingScalar easing_easy(EasingScalar p)
{
    return p * p * (3.0f - 2.0f * p);
}

EasingScalar easing_easy_slow(EasingScalar p)
{
    p = easing_easy(p);
    return p * p * (3.0f - 2.0f * p);
}

EasingScalar easing_extend(EasingScalar p)
{
    p = (p * 1.4f) - 0.2f;
    return 0.5f - std::cos(detail::pi<EasingScalar>() * p) / NEGCOSPI;
}

static inline EasingScalar power(EasingScalar base, EasingScalar exponent)
{
    if (detail::float_equal(exponent, 0.0f))
        return 1.0f;
    else if (detail::float_equal(base, 0.0f) && exponent > 0.0f)
        return 0.0f;

    return std::exp(exponent * std::log(base));
}

EasingScalar easing_drop(EasingScalar p)
{
    return p * p;
}

EasingScalar easing_drop_slow(EasingScalar p)
{
    return p * p * p * p * p;
}

EasingScalar easing_snap(EasingScalar p)
{
    p = p * p;
    p = (p * 1.4f) - 0.2f;
    return 0.5f - std::cos(detail::pi<EasingScalar>() * p) / NEGCOSPI;
}

EasingScalar easing_bounce(EasingScalar p)
{
    if (p > 0.9f)
    {
        const EasingScalar result = p - 0.95f;
        return 1.0f + result * result * 20.0f - (0.05f * 0.05f * 20.0f);
    }
    else if (p > 0.75f)
    {
        const EasingScalar result = p - 0.825f;
        return 1.0f + result * result * 16.0f - (0.075f * 0.075f * 16.0f);
    }
    else if (p > 0.5f)
    {
        const EasingScalar result = p - 0.625f;
        return 1.0f + result * result * 12.0f - (0.125f * 0.125f * 12.0f);
    }

    p = p * 2.0f;
    return p * p;
}

EasingScalar easing_bouncy(EasingScalar p)
{
    EasingScalar result = 1.0f;
    const EasingScalar scale = 5.0f;
    EasingScalar start = 0.5f;
    EasingScalar step = 0.2f;

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

EasingScalar easing_rubber(EasingScalar p)
{
    if (p > 0.9f)
    {
        const EasingScalar result = p - 0.95f;
        return 1.0f - result * result * 20.0f + (0.05f * 0.05f * 20.0f);
    }
    else if (p > 0.75f)
    {
        const EasingScalar result = p - 0.825f;
        return 1.0f + result * result * 18.0f - (0.075f * 0.075f * 18.0f);
    }
    else if (p > 0.5f)
    {
        const EasingScalar result = p - 0.625f;
        return 1.0f - result * result * 14.0f + (0.125f * 0.125f * 14.0f);
    }

    p = p * 2.0f;
    return p * p;
}

EasingScalar easing_spring(EasingScalar p)
{
    p = p * p;
    EasingScalar result = std::sin(detail::pi<EasingScalar>() * p * p * 10.0f - detail::pi<EasingScalar>() / 2.0f) / 4.0f;
    result = result * (1.0f - p) + 1.0f;
    if (p < 0.3f)
        result = result * easing_easy(p / 0.3f);
    return result;
}

EasingScalar easing_boing(EasingScalar p)
{
    p = power(p, 1.5f);
    EasingScalar result = std::sin(detail::pi<EasingScalar>() * power(p, 2.0f) * 20.0f - detail::pi<EasingScalar>() / 2.0f) / 4.0f;
    result = result * (1.0f - p) + 1.0f;
    if (p < 0.2f)
        result = result * easing_easy(p / 0.2f);
    return result;
}

EasingScalar easing_quadratic_easein(EasingScalar p)
{
    return p * p;
}

EasingScalar easing_quadratic_easeout(EasingScalar p)
{
    return -(p * (p - 2));
}

EasingScalar easing_quadratic_easeinout(EasingScalar p)
{
    if (p < 0.5f)
    {
        return 2 * p * p;
    }
    else
    {
        return (-2 * p * p) + (4 * p) - 1;
    }
}

EasingScalar easing_cubic_easein(EasingScalar p)
{
    return p * p * p;
}

EasingScalar easing_cubic_easeout(EasingScalar p)
{
    EasingScalar f = (p - 1);
    return f * f * f + 1;
}

EasingScalar easing_cubic_easeinout(EasingScalar p)
{
    if (p < 0.5f)
    {
        return 4 * p * p * p;
    }
    else
    {
        EasingScalar f = ((2 * p) - 2);
        return 0.5f * f * f * f + 1;
    }
}

EasingScalar easing_quartic_easein(EasingScalar p)
{
    return p * p * p * p;
}

EasingScalar easing_quartic_easeout(EasingScalar p)
{
    EasingScalar f = (p - 1);
    return f * f * f * (1 - p) + 1;
}

EasingScalar easing_quartic_easeinout(EasingScalar p)
{
    if (p < 0.5f)
    {
        return 8 * p * p * p * p;
    }
    else
    {
        EasingScalar f = (p - 1);
        return -8 * f * f * f * f + 1;
    }
}

EasingScalar easing_quintic_easein(EasingScalar p)
{
    return p * p * p * p * p;
}

EasingScalar easing_quintic_easeout(EasingScalar p)
{
    EasingScalar f = (p - 1);
    return f * f * f * f * f + 1;
}

EasingScalar easing_quintic_easeinout(EasingScalar p)
{
    if (p < 0.5f)
    {
        return 16 * p * p * p * p * p;
    }
    else
    {
        EasingScalar f = ((2 * p) - 2);
        return  0.5f * f * f * f * f * f + 1;
    }
}

EasingScalar easing_sine_easein(EasingScalar p)
{
    return std::sin((p - 1) * detail::pi_2<EasingScalar>()) + 1;
}

EasingScalar easing_sine_easeout(EasingScalar p)
{
    return std::sin(p * detail::pi_2<EasingScalar>());
}

EasingScalar easing_sine_easeinout(EasingScalar p)
{
    return 0.5f * (1 - std::cos(p * detail::pi<EasingScalar>()));
}

EasingScalar easing_circular_easein(EasingScalar p)
{
    return 1 - std::sqrt(1 - (p * p));
}

EasingScalar easing_circular_easeout(EasingScalar p)
{
    return std::sqrt((2 - p) * p);
}

EasingScalar easing_circular_easeinout(EasingScalar p)
{
    if (p < 0.5f)
    {
        return 0.5f * (1 - std::sqrt(1 - 4 * (p * p)));
    }
    else
    {
        return 0.5f * (std::sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
    }
}

EasingScalar easing_exponential_easein(EasingScalar p)
{
    return detail::float_equal(p, 0.0f) ? p : std::pow(2.0f, 10 * (p - 1));
}

EasingScalar easing_exponential_easeout(EasingScalar p)
{
    return detail::float_equal(p, 1.0f) ? p : 1 - std::pow(2.0f, -10 * p);
}

EasingScalar easing_exponential_easeinout(EasingScalar p)
{
    if (detail::float_equal(p, 0.0f) || detail::float_equal(p, 1.0f))
        return p;

    if (p < 0.5f)
    {
        return 0.5f * std::pow(2.0f, (20 * p) - 10);
    }
    else
    {
        return -0.5f * std::pow(2.0f, (-20 * p) + 10) + 1;
    }
}

EasingScalar easing_cubic_bezier::operator()(EasingScalar p)
{
    return std::pow(1.0f - p, 3.0f) * m_p0 +
           3.0f * p * std::pow(1.0f - p, 2.0f) * m_p1 +
           3.0f * std::pow(p, 2.0f) * (1.0f - p) * m_p2 +
           std::pow(p, 3.0f) * m_p3;
}

}
}
