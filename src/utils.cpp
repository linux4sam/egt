/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mui/utils.h"
#include "lua/script.h"

namespace mui
{
    namespace detail
    {
        int& globalloglevel()
        {
            static int loglevel = getenv("MUI_DEBUG") ? std::atoi(getenv("MUI_DEBUG")) : 0;
            return loglevel;
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

    namespace experimental
    {
        double lua_evaluate(const std::string& expr)
        {
            int cookie;
            char* msg = NULL;
            double y = 0.;

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

            return y;
        }
    }
}
