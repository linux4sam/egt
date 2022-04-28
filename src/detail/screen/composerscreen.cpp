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

Signal<> ComposerScreen::on_screen_resized;

ComposerScreen::ComposerScreen(const Size& size)
{
    detail::info("Composer Screen");

    detail::info("fb size {}", size);

    init(size);
}

void ComposerScreen::resize(const Size& size)
{
    if (m_size != size)
    {
        init(size);

        on_screen_resized.invoke();
    }
}

unsigned char* ComposerScreen::get_pixmap()
{
    return cairo_image_surface_get_data(m_surface.get());
}


Signal<>::RegisterHandle ComposerScreen::register_screen_resize_hook(const Signal<>::EventCallback& handler)
{
    return on_screen_resized(handler);
}

void ComposerScreen::unregister_screen_resize_hook(Signal<>::RegisterHandle handle)
{
    if (handle != Signal<>::INVALID_HANDLE)
        on_screen_resized.remove(handle);
}

}
}
}
