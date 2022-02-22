/*
 * Copyright (C) 2021 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "egt/detail/screen/composerscreen.h"

namespace egt
{
inline namespace v1
{
namespace detail
{

ComposerScreen::ComposerScreen(const Size& size)
{
    detail::info("Composer Screen");

    detail::info("fb size {}", size);

    init(size);
}

void ComposerScreen::resize(const Size& size)
{
    init(size);
}

unsigned char* ComposerScreen::get_pixmap()
{
    return cairo_image_surface_get_data(m_surface.get());
}

}
}
}
