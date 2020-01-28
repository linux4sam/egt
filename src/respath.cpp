/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/respath.h"
#include "egt/uri.h"
#include <cstdlib>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

namespace egt
{
inline namespace v1
{

static std::vector<std::string> image_paths;

void add_search_path(const std::string& path)
{
    if (path.empty())
        return;

    auto newpath = path;

    if (*newpath.rbegin() != '/')
        newpath += '/';

    if (find(image_paths.begin(), image_paths.end(), newpath) == image_paths.end())
        image_paths.push_back(newpath);
}

std::string resolve_file_path(const std::string& filename)
{
    if (filename.empty())
        return filename;

    // we don't resolve absolute paths
    if (filename[0] == '/')
        return filename;

    for (auto& path : image_paths)
    {
        auto test = path + filename;

        SPDLOG_TRACE("looking at file {}", test);

        struct stat buf {};
        if (!stat(test.c_str(), &buf))
        {
            SPDLOG_TRACE("found file {}", test);
            return test;
        }
    }

    return filename;
}

namespace detail
{
SchemeType resolve_path(const std::string& path, std::string& result)
{
    SchemeType type = SchemeType::unknown;

    Uri uri(path);

    if (uri.scheme() == "res")
    {
        type = SchemeType::resource;
        result = uri.path();
    }
    else if (uri.scheme() == "icon")
    {
        type = SchemeType::filesystem;
        result = uri.path();

        static std::string default_egt_icons_dir = "32px";
        static std::once_flag env_flag;
        std::call_once(env_flag, []()
        {
            auto icons_dir = std::getenv("EGT_ICONS_DIRECTORY");
            if (icons_dir)
            {
                auto dir = std::string(icons_dir);
                if (!dir.empty())
                    default_egt_icons_dir = dir;
            }
        });

        if (result.find('/') != std::string::npos)
            result = resolve_file_path(result);
        else
        {
            auto egt_icons_dir = default_egt_icons_dir;
            if (!uri.icon_size().empty())
            {
                egt_icons_dir = std::stoi(uri.icon_size()) + "px";
            }
            result = resolve_file_path(egt_icons_dir + "/" + result);
        }
    }
    else if (uri.scheme() == "http" || uri.scheme() == "https")
    {
        type = SchemeType::network;
        result = uri.to_string();
    }
    else if (uri.scheme() == "file")
    {
        type = SchemeType::filesystem;
        result = resolve_file_path(uri.path());
    }
    else
    {
        spdlog::warn("unable to parse uri: {}", path);
    }

    return type;
}
}
}
}
