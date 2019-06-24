/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/floatingpoint.h"
#include "egt/geometry.h"

namespace egt
{
inline namespace v1
{

template<class dim_t>
bool ArcType<dim_t>::empty() const
{
    return this->radius <= 0.0f ||
           detail::FloatingPoint<float>(this->radius).
           AlmostEquals(detail::FloatingPoint<float>(0.0f));
}

template bool ArcType<float>::empty() const;
template bool ArcType<int>::empty() const;

}
}
