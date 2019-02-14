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
 * The BasicWindow backend uses software to create a Screen.
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
