/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_BASICWINDOW_H
#define EGT_DETAIL_BASICWINDOW_H

/**
 * @file
 * @brief Working with windows.
 */

#include <egt/window.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * A BasicWindow is a Frame Widget that contains and is backed by a Screen.
 *
 * Any top level widget must be a BasicWindow.
 *
 * Windows, unlike other basic widgets, are hidden by default. They will always
 * require a call to show().
 */
class BasicWindow : public detail::WindowImpl
{
public:

    explicit BasicWindow(Window* interface);

    virtual ~BasicWindow();
};

/**
 * A BasicTopWindow will grab the main_screen() by default.
 */
class BasicTopWindow : public BasicWindow
{
public:

    explicit BasicTopWindow(Window* interface);

    virtual ~BasicTopWindow();
};

}
}
}

#endif
