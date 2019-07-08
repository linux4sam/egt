/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "egt/detail/screen/memoryscreen.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

MemoryScreen::MemoryScreen(const Size& size)
    : m_canvas(size)
{
    detail::info("Memory Screen");

    detail::info("fb size {}", size);

    init(size);
}

void MemoryScreen::save_to_file(const std::string& filename) const
{
    cairo_surface_write_to_png(m_canvas.surface().get(), filename.c_str());
}

}
}
}
