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

BasicWindow::BasicWindow(Window* interface)
    : WindowImpl(interface)
{

}

BasicWindow::~BasicWindow()
{
}

BasicTopWindow::BasicTopWindow(Window* interface)
    : BasicWindow(interface)
{
    // go ahead and pick up the default screen
    assert(main_screen());
    m_screen = main_screen();
}

BasicTopWindow::~BasicTopWindow()
{
}

}
}
}
