/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/window/basicwindow.h"
#include "egt/screen.h"
#include <cassert>

namespace egt
{
inline namespace v1
{
namespace detail
{

BasicWindow::BasicWindow(Window* interface, Screen* screen)
    : WindowImpl(interface, screen)
{

}

BasicTopWindow::BasicTopWindow(Window* interface)
    : BasicWindow(interface, main_screen())
{
    // go ahead and pick up the default screen
    assert(main_screen());
}

}
}
}
