/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_WINDOW_BASICWINDOW_H
#define EGT_DETAIL_WINDOW_BASICWINDOW_H

#include "detail/window/windowimpl.h"
#include "egt/window.h"

namespace egt
{
inline namespace v1
{
class Screen;

namespace detail
{

/**
 * The BasicWindow backend uses software to create a Screen.
 */
class BasicWindow : public detail::WindowImpl
{
public:

    explicit BasicWindow(Window* inter, Screen* screen = nullptr);

    virtual ~BasicWindow() = default;
};

/**
 * A BasicTopWindow will grab the main screen by default.
 */
class BasicTopWindow : public BasicWindow
{
public:

    explicit BasicTopWindow(Window* inter);

    virtual ~BasicTopWindow() = default;
};

}
}
}

#endif
