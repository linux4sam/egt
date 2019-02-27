/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_TYPES_H
#define EGT_TYPES_H

/**
 * @file
 * @brief Common types.
 */

#include <cairo.h>
#include <memory>

namespace egt
{
inline namespace v1
{

using shared_cairo_surface_t =
    std::shared_ptr<cairo_surface_t>;

using shared_cairo_t =
    std::shared_ptr<cairo_t>;

using shared_cairo_pattern_t =
    std::shared_ptr<cairo_pattern_t>;

namespace detail
{
struct cairo_t_deleter
{
    void operator()(cairo_t* cr) { cairo_destroy(cr); }
};
}

using unique_cairo_t =
    std::unique_ptr<cairo_t, detail::cairo_t_deleter>;

}
}

#endif
