/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/respath.h"
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <sys/types.h>
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

}
}
