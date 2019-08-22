/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/utils.h"
#include <cstring>
#include <memory>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <unistd.h>

#ifdef HAVE_LUA
#include "lua/script.h"
#endif

namespace egt
{
inline namespace v1
{
namespace experimental
{

double lua_evaluate(const std::string& expr)
{
    double y = 0.;
#ifdef HAVE_LUA
    int cookie;
    char* msg = nullptr;

    if (!script_init(nullptr))
    {
        spdlog::error("can't init lua");
        return y;
    }
    cookie = script_load(expr.c_str(), &msg);
    if (msg)
    {
        spdlog::error("can't load expr: {}", msg);
        goto error;
    }

    y = script_eval(cookie, &msg);
    if (msg)
    {
        spdlog::error("can't eval: {}", msg);
        goto error;
    }

error:
    if (msg)
        free(msg);
    script_unref(cookie);
    script_close();
#else
    ignoreparam(expr);
#endif
    return y;
}

}
}
}
