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

#include <egt/detail/meta.h>

namespace egt
{
inline namespace v1
{
using float_t = float;

/**
 * @defgroup easing_functions Animation Easing Functions
 * These functions and functors define easing curves for use with animations.
 * @see @ref animations
 */

/**
 * @addtogroup easing_functions
 * @{
 */

/**
 * @imageSize{linear.png,width:320px;}
 * @image html easing/linear.png
 * @image latex easing/linear.png
 */
EGT_API float_t easing_linear(float_t p);
/**
 * @imageSize{easy.png,width:320px;}
 * @image html easing/easy.png
 * @image latex easing/easy.png
 */
EGT_API float_t easing_easy(float_t p);
/**
 * @imageSize{easy_slow.png,width:320px;}
 * @image html easing/easy_slow.png
 * @image latex easing/easy_slow.png
 */
EGT_API float_t easing_easy_slow(float_t p);
/**
 * @imageSize{extend.png,width:320px;}
 * @image html easing/extend.png
 * @image latex easing/extend.png
 */
EGT_API float_t easing_extend(float_t p);
/**
 * @imageSize{drop.png,width:320px;}
 * @image html easing/drop.png
 * @image latex easing/drop.png
 */
EGT_API float_t easing_drop(float_t p);
/**
 * @imageSize{drop_slow.png,width:320px;}
 * @image html easing/drop_slow.png
 * @image latex easing/drop_slow.png
 */
EGT_API float_t easing_drop_slow(float_t p);
/**
 * @imageSize{snap.png,width:320px;}
 * @image html easing/snap.png
 * @image latex easing/snap.png
 */
EGT_API float_t easing_snap(float_t p);
/**
 * @imageSize{bounce.png,width:320px;}
 * @image html easing/bounce.png
 * @image latex easing/bounce.png
 */
EGT_API float_t easing_bounce(float_t p);
/**
 * @imageSize{bouncy.png,width:320px;}
 * @image html easing/bouncy.png
 * @image latex easing/bouncy.png
 */
EGT_API float_t easing_bouncy(float_t p);
/**
 * @imageSize{rubber.png,width:320px;}
 * @image html easing/rubber.png
 * @image latex easing/rubber.png
 */
EGT_API float_t easing_rubber(float_t p);
/**
 * @imageSize{spring.png,width:320px;}
 * @image html easing/spring.png
 * @image latex easing/spring.png
 */
EGT_API float_t easing_spring(float_t p);
/**
 * @imageSize{boing.png,width:320px;}
 * @image html easing/boing.png
 * @image latex easing/boing.png
 */
EGT_API float_t easing_boing(float_t p);
/**
 * @imageSize{quadratic_easein.png,width:320px;}
 * @image html easing/quadratic_easein.png
 * @image latex easing/quadratic_easein.png
 */
EGT_API float_t easing_quadratic_easein(float_t p);
/**
 * @imageSize{quadratic_easeout.png,width:320px;}
 * @image html easing/quadratic_easeout.png
 * @image latex easing/quadratic_easeout.png
 */
EGT_API float_t easing_quadratic_easeout(float_t p);
/**
 * @imageSize{quadratic_easeinout.png,width:320px;}
 * @image html easing/quadratic_easeinout.png
 * @image latex easing/quadratic_easeinout.png
 */
EGT_API float_t easing_quadratic_easeinout(float_t p);
/**
 * @imageSize{cubic_easein.png,width:320px;}
 * @image html easing/cubic_easein.png
 * @image latex easing/cubic_easein.png
 */
EGT_API float_t easing_cubic_easein(float_t p);
/**
 * @imageSize{cubic_easeout.png,width:320px;}
 * @image html easing/cubic_easeout.png
 * @image latex easing/cubic_easeout.png
 */
EGT_API float_t easing_cubic_easeout(float_t p);
/**
 * @imageSize{cubic_easeinout.png,width:320px;}
 * @image html easing/cubic_easeinout.png
 * @image latex easing/cubic_easeinout.png
 */
EGT_API float_t easing_cubic_easeinout(float_t p);
/**
 * @imageSize{quartic_easein.png,width:320px;}
 * @image html easing/quartic_easein.png
 * @image latex easing/quartic_easein.png
 */
EGT_API float_t easing_quartic_easein(float_t p);
/**
 * @imageSize{quartic_easeout.png,width:320px;}
 * @image html easing/quartic_easeout.png
 * @image latex easing/quartic_easeout.png
 */
EGT_API float_t easing_quartic_easeout(float_t p);
/**
 * @imageSize{quartic_easeinout.png,width:320px;}
 * @image html easing/quartic_easeinout.png
 * @image latex easing/quartic_easeinout.png
 */
EGT_API float_t easing_quartic_easeinout(float_t p);
/**
 * @imageSize{quintic_easein.png,width:320px;}
 * @image html easing/quintic_easein.png
 * @image latex easing/quintic_easein.png
 */
EGT_API float_t easing_quintic_easein(float_t p);
/**
 * @imageSize{quintic_easeout.png,width:320px;}
 * @image html easing/quintic_easeout.png
 * @image latex easing/quintic_easeout.png
 */
EGT_API float_t easing_quintic_easeout(float_t p);
/**
 * @imageSize{quintic_easeinout.png,width:320px;}
 * @image html easing/quintic_easeinout.png
 * @image latex easing/quintic_easeinout.png
 */
EGT_API float_t easing_quintic_easeinout(float_t p);
/**
 * @imageSize{sine_easein.png,width:320px;}
 * @image html easing/sine_easein.png
 * @image latex easing/sine_easein.png
 */
EGT_API float_t easing_sine_easein(float_t p);
/**
 * @imageSize{sine_easeout.png,width:320px;}
 * @image html easing/sine_easeout.png
 * @image latex easing/sine_easeout.png
 */
EGT_API float_t easing_sine_easeout(float_t p);
/**
 * @imageSize{sine_easeinout.png,width:320px;}
 * @image html easing/sine_easeinout.png
 * @image latex easing/sine_easeinout.png
 */
EGT_API float_t easing_sine_easeinout(float_t p);
/**
 * @imageSize{circular_easein.png,width:320px;}
 * @image html easing/circular_easein.png
 * @image latex easing/circular_easein.png
 */
EGT_API float_t easing_circular_easein(float_t p);
/**
 * @imageSize{circular_easeout.png,width:320px;}
 * @image html easing/circular_easeout.png
 * @image latex easing/circular_easeout.png
 */
EGT_API float_t easing_circular_easeout(float_t p);
/**
 * @imageSize{circular_easeinout.png,width:320px;}
 * @image html easing/circular_easeinout.png
 * @image latex easing/circular_easeinout.png
 */
EGT_API float_t easing_circular_easeinout(float_t p);
/**
 * @imageSize{exponential_easein.png,width:320px;}
 * @image html easing/exponential_easein.png
 * @image latex easing/exponential_easein.png
 */
EGT_API float_t easing_exponential_easein(float_t p);
/**
 * @imageSize{exponential_easeout.png,width:320px;}
 * @image html easing/exponential_easeout.png
 * @image latex easing/exponential_easeout.png
 */
EGT_API float_t easing_exponential_easeout(float_t p);
/**
 * @imageSize{exponential_easeinout.png,width:320px;}
 * @image html easing/exponential_easeinout.png
 * @image latex easing/exponential_easeinout.png
 */
EGT_API float_t easing_exponential_easeinout(float_t p);

/**
 * Cubic Bezier equation easing function.
 *
 * @imageSize{cubic_bezier.png,width:320px;}
 * @image html easing/cubic_bezier.png
 * @image latex easing/cubic_bezier.png
 * @ingroup easing_functions
 */
struct EGT_API easing_cubic_bezier
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

    float_t m_p0{};
    float_t m_p1{};
    float_t m_p2{};
    float_t m_p3{};
};

/** @} */

}
}

#endif
