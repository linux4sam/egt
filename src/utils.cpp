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
#include <stdexcept>
#include <unistd.h>

#ifdef HAVE_LUA
#include "lua/script.h"
#endif

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

int& globalloglevel()
{
    static int loglevel = getenv("EGT_DEBUG") ? std::atoi(getenv("EGT_DEBUG")) : 0;
    return loglevel;
}

}

namespace experimental
{

double lua_evaluate(const std::string& expr)
{
    double y = 0.;
#ifdef HAVE_LUA
    int cookie;
    char* msg = NULL;


    if (!script_init(nullptr))
    {
        ERR("can't init lua");
        return y;
    }
    cookie = script_load(expr.c_str(), &msg);
    if (msg)
    {
        ERR("can't load expr: " << msg);
        goto error;
    }

    y = script_eval(cookie, &msg);
    if (msg)
    {
        ERR("can't eval: " << msg);
        goto error;
    }

error:
    if (msg)
        free(msg);
    script_unref(cookie);
#else
    ignoreparam(expr);
#endif
    return y;
}

}
}
}
