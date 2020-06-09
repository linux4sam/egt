/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/string.h"
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

static void normalize_dir(std::string& dir)
{
    if (dir.empty())
        return;

    if (*dir.rbegin() != '/')
        dir += '/';
}

void add_search_path(const std::string& path)
{
    if (path.empty())
        return;

    auto newpath = path;
    normalize_dir(newpath);

    if (find(image_paths.begin(), image_paths.end(), newpath) == image_paths.end())
        image_paths.push_back(newpath);
}

static std::string resolve_file_path(const std::string& filename,
                                     const std::vector<std::string>& search)
{
    if (filename.empty())
        return filename;

    // we don't resolve absolute paths
    if (filename[0] == '/')
        return filename;

    for (auto& path : search)
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

std::string resolve_file_path(const std::string& filename)
{
    return resolve_file_path(filename, image_paths);
}

namespace detail
{
SchemeType resolve_path(const std::string& path, std::string& result)
{
    SchemeType type = SchemeType::unknown;

    Uri uri(path);

    switch (detail::hash(uri.scheme()))
    {
    case detail::hash("res"):
    {
        type = SchemeType::resource;
        result = uri.path();
        break;
    }
    case detail::hash("icon"):
    {
        type = SchemeType::filesystem;
        result = uri.path();

        static std::string egt_icons_dir =
            std::string(DATADIR) + "/libegt/icons/";
        static std::once_flag env_flag;
        std::call_once(env_flag, []()
        {
            auto dir = std::getenv("EGT_ICONS_DIRECTORY");
            if (dir)
            {
                auto icons_dir = std::string(dir);
                if (!icons_dir.empty())
                {
                    egt_icons_dir = icons_dir;
                    normalize_dir(egt_icons_dir);
                }
            }
        });

        std::string icon_prefix = "32px";
        if (!uri.icon_size().empty())
            icon_prefix = uri.icon_size() + "px";
        result = resolve_file_path(icon_prefix + "/" + result, {egt_icons_dir});
        break;
    }
    case detail::hash("http"):
    case detail::hash("https"):
    case detail::hash("rtsp"):
    {
        type = SchemeType::network;
        result = uri.to_string();
        break;
    }
    case detail::hash("file"):
    {
        type = SchemeType::filesystem;
        result = resolve_file_path(uri.path());
        break;
    }
    default:
    {
        break;
    }
    }

    return type;
}
}

}
}
