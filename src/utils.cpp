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
#include <glob.h>
#include <libgen.h>
#include <limits.h>
#include <memory>
#include <sstream>
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

static std::string dirname(const std::string& path)
{
    unique_ptr<char, decltype(std::free)*> p(strdup(path.c_str()), std::free);
    auto dir = ::dirname(p.get());
    return std::string(dir);
}

static std::string readlink(const std::string& path)
{
    char buff[PATH_MAX];
    ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff) - 1);
    if (len != -1)
    {
        buff[len] = '\0';
        return std::string(buff);
    }
    return std::string();
}

std::string exe_pwd()
{
    return dirname(std::string(readlink("/proc/self/exe")));
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

}
}
}
