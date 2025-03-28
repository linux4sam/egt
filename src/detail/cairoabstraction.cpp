/*
 * Copyright (C) 2024 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/cairoabstraction.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

static const std::pair<Painter::LineCap, cairo_line_cap_t> cairo_line_caps[] =
{
    {Painter::LineCap::butt, CAIRO_LINE_CAP_BUTT},
    {Painter::LineCap::round, CAIRO_LINE_CAP_ROUND},
    {Painter::LineCap::square, CAIRO_LINE_CAP_SQUARE},
};

cairo_line_cap_t cairo_line_cap(Painter::LineCap line_cap)
{
    for (const auto& i : cairo_line_caps)
        if (i.first == line_cap)
            return i.second;

    return CAIRO_LINE_CAP_BUTT;
}

Painter::LineCap egt_line_cap(cairo_line_cap_t line_cap)
{
    for (const auto& i : cairo_line_caps)
        if (i.second == line_cap)
            return i.first;

    return Painter::LineCap::butt;
}

}
}
}
