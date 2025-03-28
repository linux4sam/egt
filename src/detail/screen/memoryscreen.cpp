/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "egt/detail/screen/memoryscreen.h"
#include <cairo.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

MemoryScreen::MemoryScreen(const Size& size)
{
    detail::info("Memory Screen");

    detail::info("fb size {}", size);

    init(size);
}

void MemoryScreen::save_to_file(const std::string& filename) const
{
#if CAIRO_HAS_PNG_FUNCTIONS == 1
    cairo_surface_write_to_png(m_surface.get(), filename.c_str());
#else
    detail::ignoreparam(filename);
    detail::error("png support not available");
#endif
}

}
}
}
