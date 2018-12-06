/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/utils.h"
#include <glob.h>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <chrono>

#ifdef HAVE_LUA
#include "lua/script.h"
#endif

using namespace std;

namespace egt
{
    namespace detail
    {
        int& globalloglevel()
        {
            static int loglevel = getenv("EGT_DEBUG") ? std::atoi(getenv("EGT_DEBUG")) : 0;
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
#endif
            return y;
        }

        std::vector<std::string> glob(const std::string& pattern)
        {
            glob_t glob_result;
            memset(&glob_result, 0, sizeof(glob_result));

            int return_value = glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
            if (return_value != 0)
            {
                globfree(&glob_result);
                std::stringstream ss;
                ss << "glob() failed: " << return_value << std::endl;
                throw std::runtime_error(ss.str());
            }

            std::vector<std::string> filenames;
            for (size_t i = 0; i < glob_result.gl_pathc; ++i)
                filenames.push_back(std::string(glob_result.gl_pathv[i]));

            globfree(&glob_result);

            return filenames;
        }

        //#define ENABLE_CODE_TIMER
        void code_timer(const std::string& prefix, std::function<void ()> callback)
        {
#ifdef ENABLE_CODE_TIMER
            auto start = chrono::steady_clock::now();
#else
            ignoreparam(prefix);
#endif
            callback();

#ifdef ENABLE_CODE_TIMER
            auto end = chrono::steady_clock::now();
            auto diff = end - start;

            cout << prefix << chrono::duration<double, milli>(diff).count() << endl;
#endif
        }

    }
}
