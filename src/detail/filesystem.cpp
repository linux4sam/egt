/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/detail/filesystem.h"
#include <climits>
#include <cstring>
#include <fstream>
#include <glob.h>
#include <iterator>
#include <libgen.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_EXPERIMENTAL_FILESYSTEM
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

namespace egt
{
inline namespace v1
{
namespace detail
{

std::string extract_filename(const std::string& path)
{
    const char sep = '/';

    const size_t i = path.rfind(sep, path.length());
    if (i != std::string::npos)
        return (path.substr(i + 1, path.length() - i));

    return {};
}

std::string extract_dirname(const std::string& path)
{
    std::unique_ptr<char, decltype(std::free)*> p(strdup(path.c_str()), std::free);
    const auto dir = ::dirname(p.get());
    return std::string(dir);
}

bool exists(const std::string& path)
{
    struct stat buf {};
    return !stat(path.c_str(), &buf);
}

std::vector<unsigned char> read_file(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ios::binary);

    file.unsetf(std::ios::skipws);
    std::streampos filesize;

    file.seekg(0, std::ios::end);
    filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> vec;
    vec.reserve(filesize);

    vec.insert(vec.begin(),
               std::istream_iterator<unsigned char>(file),
               std::istream_iterator<unsigned char>());

    return vec;
}

std::string readlink(const std::string& path)
{
    char buff[PATH_MAX];
    ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff) - 1);
    if (len != -1)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        buff[len] = '\0';
        return std::string(buff);
    }
    return {};
}

std::string abspath(const std::string& path)
{
    char buff[PATH_MAX];
    if (::realpath(path.c_str(), buff))
        return std::string(buff);

    return {};
}

std::string exe_pwd()
{
    return extract_dirname(std::string(readlink("/proc/self/exe")));
}

std::string cwd()
{
#ifdef HAVE_EXPERIMENTAL_FILESYSTEM
    return fs::current_path();
#else
    std::unique_ptr<char, decltype(std::free)*> d(get_current_dir_name(), std::free);
    if (d)
        return d.get();
#endif
    return {};
}

std::vector<std::string> glob(const std::string& pattern)
{
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));

    const auto return_value = glob(pattern.c_str(), GLOB_TILDE, nullptr, &glob_result);
    if (return_value != 0)
    {
        globfree(&glob_result);
        throw std::runtime_error(fmt::format("glob() failed: {}", return_value));
    }

    std::vector<std::string> filenames;
    for (size_t i = 0; i < glob_result.gl_pathc; ++i)
        filenames.emplace_back(glob_result.gl_pathv[i]);

    globfree(&glob_result);

    return filenames;
}

}
}
}
