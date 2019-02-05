/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/detail/basicwindow.h"

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
