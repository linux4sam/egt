/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_LUAAPP_H
#define EGT_LUAAPP_H

#include <egt/app.h>
#include <egt/detail/meta.h>

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
class EGT_API LuaApplication : public egt::Application
{
public:

    using egt::Application::Application;

    virtual int run() override;

    virtual ~LuaApplication();
};

}
}
}

#endif
