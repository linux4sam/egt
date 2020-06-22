/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/eraw.h"
#include "detail/erawimage.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

shared_cairo_surface_t load_eraw(const std::string& filename)
{
    ErawImage e;
    return e.load(filename);
}

shared_cairo_surface_t load_eraw(const unsigned char* buf, size_t len)
{
    ErawImage e;
    return e.load(buf, len);
}

}
}
}
