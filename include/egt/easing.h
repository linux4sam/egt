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
using EasingScalar = float;

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
EGT_API EasingScalar easing_linear(EasingScalar p);
/**
 * @imageSize{easy.png,width:320px;}
 * @image html easing/easy.png
 * @image latex easing/easy.png
 */
EGT_API EasingScalar easing_easy(EasingScalar p);
/**
 * @imageSize{easy_slow.png,width:320px;}
 * @image html easing/easy_slow.png
 * @image latex easing/easy_slow.png
 */
EGT_API EasingScalar easing_easy_slow(EasingScalar p);
/**
 * @imageSize{extend.png,width:320px;}
 * @image html easing/extend.png
 * @image latex easing/extend.png
 */
EGT_API EasingScalar easing_extend(EasingScalar p);
/**
 * @imageSize{drop.png,width:320px;}
 * @image html easing/drop.png
 * @image latex easing/drop.png
 */
EGT_API EasingScalar easing_drop(EasingScalar p);
/**
 * @imageSize{drop_slow.png,width:320px;}
 * @image html easing/drop_slow.png
 * @image latex easing/drop_slow.png
 */
EGT_API EasingScalar easing_drop_slow(EasingScalar p);
/**
 * @imageSize{snap.png,width:320px;}
 * @image html easing/snap.png
 * @image latex easing/snap.png
 */
EGT_API EasingScalar easing_snap(EasingScalar p);
/**
 * @imageSize{bounce.png,width:320px;}
 * @image html easing/bounce.png
 * @image latex easing/bounce.png
 */
EGT_API EasingScalar easing_bounce(EasingScalar p);
/**
 * @imageSize{bouncy.png,width:320px;}
 * @image html easing/bouncy.png
 * @image latex easing/bouncy.png
 */
EGT_API EasingScalar easing_bouncy(EasingScalar p);
/**
 * @imageSize{rubber.png,width:320px;}
 * @image html easing/rubber.png
 * @image latex easing/rubber.png
 */
EGT_API EasingScalar easing_rubber(EasingScalar p);
/**
 * @imageSize{spring.png,width:320px;}
 * @image html easing/spring.png
 * @image latex easing/spring.png
 */
EGT_API EasingScalar easing_spring(EasingScalar p);
/**
 * @imageSize{boing.png,width:320px;}
 * @image html easing/boing.png
 * @image latex easing/boing.png
 */
EGT_API EasingScalar easing_boing(EasingScalar p);
/**
 * @imageSize{quadratic_easein.png,width:320px;}
 * @image html easing/quadratic_easein.png
 * @image latex easing/quadratic_easein.png
 */
EGT_API EasingScalar easing_quadratic_easein(EasingScalar p);
/**
 * @imageSize{quadratic_easeout.png,width:320px;}
 * @image html easing/quadratic_easeout.png
 * @image latex easing/quadratic_easeout.png
 */
EGT_API EasingScalar easing_quadratic_easeout(EasingScalar p);
/**
 * @imageSize{quadratic_easeinout.png,width:320px;}
 * @image html easing/quadratic_easeinout.png
 * @image latex easing/quadratic_easeinout.png
 */
EGT_API EasingScalar easing_quadratic_easeinout(EasingScalar p);
/**
 * @imageSize{cubic_easein.png,width:320px;}
 * @image html easing/cubic_easein.png
 * @image latex easing/cubic_easein.png
 */
EGT_API EasingScalar easing_cubic_easein(EasingScalar p);
/**
 * @imageSize{cubic_easeout.png,width:320px;}
 * @image html easing/cubic_easeout.png
 * @image latex easing/cubic_easeout.png
 */
EGT_API EasingScalar easing_cubic_easeout(EasingScalar p);
/**
 * @imageSize{cubic_easeinout.png,width:320px;}
 * @image html easing/cubic_easeinout.png
 * @image latex easing/cubic_easeinout.png
 */
EGT_API EasingScalar easing_cubic_easeinout(EasingScalar p);
/**
 * @imageSize{quartic_easein.png,width:320px;}
 * @image html easing/quartic_easein.png
 * @image latex easing/quartic_easein.png
 */
EGT_API EasingScalar easing_quartic_easein(EasingScalar p);
/**
 * @imageSize{quartic_easeout.png,width:320px;}
 * @image html easing/quartic_easeout.png
 * @image latex easing/quartic_easeout.png
 */
EGT_API EasingScalar easing_quartic_easeout(EasingScalar p);
/**
 * @imageSize{quartic_easeinout.png,width:320px;}
 * @image html easing/quartic_easeinout.png
 * @image latex easing/quartic_easeinout.png
 */
EGT_API EasingScalar easing_quartic_easeinout(EasingScalar p);
/**
 * @imageSize{quintic_easein.png,width:320px;}
 * @image html easing/quintic_easein.png
 * @image latex easing/quintic_easein.png
 */
EGT_API EasingScalar easing_quintic_easein(EasingScalar p);
/**
 * @imageSize{quintic_easeout.png,width:320px;}
 * @image html easing/quintic_easeout.png
 * @image latex easing/quintic_easeout.png
 */
EGT_API EasingScalar easing_quintic_easeout(EasingScalar p);
/**
 * @imageSize{quintic_easeinout.png,width:320px;}
 * @image html easing/quintic_easeinout.png
 * @image latex easing/quintic_easeinout.png
 */
EGT_API EasingScalar easing_quintic_easeinout(EasingScalar p);
/**
 * @imageSize{sine_easein.png,width:320px;}
 * @image html easing/sine_easein.png
 * @image latex easing/sine_easein.png
 */
EGT_API EasingScalar easing_sine_easein(EasingScalar p);
/**
 * @imageSize{sine_easeout.png,width:320px;}
 * @image html easing/sine_easeout.png
 * @image latex easing/sine_easeout.png
 */
EGT_API EasingScalar easing_sine_easeout(EasingScalar p);
/**
 * @imageSize{sine_easeinout.png,width:320px;}
 * @image html easing/sine_easeinout.png
 * @image latex easing/sine_easeinout.png
 */
EGT_API EasingScalar easing_sine_easeinout(EasingScalar p);
/**
 * @imageSize{circular_easein.png,width:320px;}
 * @image html easing/circular_easein.png
 * @image latex easing/circular_easein.png
 */
EGT_API EasingScalar easing_circular_easein(EasingScalar p);
/**
 * @imageSize{circular_easeout.png,width:320px;}
 * @image html easing/circular_easeout.png
 * @image latex easing/circular_easeout.png
 */
EGT_API EasingScalar easing_circular_easeout(EasingScalar p);
/**
 * @imageSize{circular_easeinout.png,width:320px;}
 * @image html easing/circular_easeinout.png
 * @image latex easing/circular_easeinout.png
 */
EGT_API EasingScalar easing_circular_easeinout(EasingScalar p);
/**
 * @imageSize{exponential_easein.png,width:320px;}
 * @image html easing/exponential_easein.png
 * @image latex easing/exponential_easein.png
 */
EGT_API EasingScalar easing_exponential_easein(EasingScalar p);
/**
 * @imageSize{exponential_easeout.png,width:320px;}
 * @image html easing/exponential_easeout.png
 * @image latex easing/exponential_easeout.png
 */
EGT_API EasingScalar easing_exponential_easeout(EasingScalar p);
/**
 * @imageSize{exponential_easeinout.png,width:320px;}
 * @image html easing/exponential_easeinout.png
 * @image latex easing/exponential_easeinout.png
 */
EGT_API EasingScalar easing_exponential_easeinout(EasingScalar p);

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

    /**
     * @param[in] p0 Point of the cubic-bezier function.
     * @param[in] p1 Point of the cubic-bezier function.
     * @param[in] p2 Point of the cubic-bezier function.
     * @param[in] p3 Point of the cubic-bezier function.
     */
    explicit constexpr easing_cubic_bezier(EasingScalar p0 = 0.42,
                                           EasingScalar p1 = 0,
                                           EasingScalar p2 = 0.58,
                                           EasingScalar p3 = 1) noexcept
        : m_p0(p0),
          m_p1(p1),
          m_p2(p2),
          m_p3(p3)
    {}

    /// Get the easing value.
    EasingScalar operator()(EasingScalar p);

    /// Point of the cubic-bezier function.
    EasingScalar m_p0{};
    /// Point of the cubic-bezier function.
    EasingScalar m_p1{};
    /// Point of the cubic-bezier function.
    EasingScalar m_p2{};
    /// Point of the cubic-bezier function.
    EasingScalar m_p3{};
};

/** @} */

}
}

#endif
