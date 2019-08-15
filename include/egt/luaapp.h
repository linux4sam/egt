/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_LUAAPP_H
#define EGT_LUAAPP_H

#include <egt/app.h>

/**
 * @file
 * @brief Working with LUA app.
 */

namespace egt
{
inline namespace v1
{
namespace experimental
{

/**
 * Application instance that provides support for running LUA, including a LUA
 * REPL.
 *
 */
class LuaApplication : public egt::Application
{
public:

    using Application::Application;

    virtual int run() override;
};

}
}
}

#endif
