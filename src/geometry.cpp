/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/math.h"
#include "egt/geometry.h"

namespace egt
{
inline namespace v1
{

template<class Dim>
bool ArcType<Dim>::empty() const
{
    return this->radius() < 0.0f ||
           detail::float_equal(this->radius(), 0.0f);
}

template bool ArcType<float>::empty() const;
template bool ArcType<int>::empty() const;

}
}
