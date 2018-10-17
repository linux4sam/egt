/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mui/utils.h"
#include "lua/script.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace mui
{

    static int m_v = 0;

    void LOG_VERBOSE(int verbose)
    {
        m_v = verbose;
    }

    static int envset = -1;

    void LOG(const char* format, ...)
    {
        if (envset < 0)
            envset = !!getenv("MUI_DEBUG");

        if (m_v || envset)
        {
            va_list ap;
            va_start(ap, format);
            vfprintf(stderr, format, ap);
            va_end(ap);
        }
    }

    int globalenvset = -1;

    namespace experimental
    {
        double lua_evaluate(const std::string& expr)
        {
            int cookie;
            char* msg = NULL;
            double y = 0.;

            if (!script_init(nullptr))
            {
                LOG("can't init lua\n");
                return y;
            }
            cookie = script_load(expr.c_str(), &msg);
            if (msg)
            {
                LOG("can't load expr: %s\n", msg);
                goto error;
            }

            y = script_eval(cookie, &msg);
            if (msg)
            {
                LOG("can't eval: %s\n", msg);
                goto error;
            }

error:
            if (msg)
                free(msg);
            script_unref(cookie);

            return y;
        }
    }

    std::string replace_all(std::string str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }
}
