/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_PAINTER_H
#define EGT_SRC_DETAIL_PAINTER_H

/**
 * @file
 * @brief Painter class private API.
 */

#include "egt/painter.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Get a Painter, which can be used to retrieve a dummy cairo_t*, for computing
 * font/text extents for instance.
 * The size of target surface behind this painter and its cairo_t* context is
 * Size(1, 1). Hence, this painter should not be used to actually draw anything.
 */
Painter& dummy_painter();

}
}
}

#endif
