/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_MATH_H
#define EGT_DETAIL_MATH_H

#include <cassert>

namespace egt
{
inline namespace v1
{
namespace detail
{
/**
 * Normalize a value, given its min and max, to a different target min and
 * max.
 *
 * @param value The value between @min and @max.
 * @param min The minimum @value.
 * @param max The maximum @value.
 * @param target_min The minimum of the range of your desired target scaling.
 * @param target_max The maximum of the range of your desired target scaling.
 *
 * @f$
 * m\mapsto \frac{m-r_{\text{min}}}{r_{\text{max}}-r_{\text{min}}}\times (t_{\text{max}}-t_{\text{min}}) + t_{\text{min}}
 * @f$
 */
template<class T>
T normalize(T value, T min, T max, T target_min, T target_max)
{
    auto r = ((value - min) / (max - min)) *
             (target_max - target_min) + target_min;
    return r;
}

}
}
}

#endif
