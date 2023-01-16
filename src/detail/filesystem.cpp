/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/fmt.h"
#include "egt/detail/filesystem.h"
#include <climits>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <libgen.h>
#include <memory>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_GLOB_H
#include <glob.h>
#endif

namespace fs = std::filesystem;

#ifdef HAVE_WINDOWS_H
#include <algorithm>
#include <cstdio>
#include <tchar.h>
#include <windows.h>
#endif

namespace egt
{
inline namespace v1
{
namespace detail
{

#ifdef HAVE_WINDOWS_H
static std::string dos2unix_path(const std::string& path)
{
    std::string p(path);
    std::replace(p.begin(), p.end(), '\\', '/');
    return p;
}
#else
#define dos2unix_path(path) (path)
#endif

std::string extract_filename(const std::string& path)
{
    const char sep = '/';

    const size_t i = path.rfind(sep, path.length());
    if (i != std::string::npos)
        return (path.substr(i + 1, path.length() - i));

    return {};
}

// can't depend on strdup() in some environments
static char* custom_strdup(const char* str)
{
    const size_t len = strlen(str) + 1;
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc, hicpp-no-malloc)
    char* buf = static_cast<char*>(malloc(len));
    if (buf)
        return static_cast<char*>(memcpy(buf, str, len));
    return buf;
}

std::string extract_dirname(const std::string& path)
{
    std::unique_ptr<char, decltype(std::free)*> p(custom_strdup(path.c_str()), std::free);
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
    std::error_code ec;
    return dos2unix_path(fs::read_symlink(path, ec).string());
}

std::string abspath(const std::string& path)
{
    std::error_code ec;
    return dos2unix_path(fs::canonical(path, ec).string());
}

std::string exe_pwd()
{
#ifdef HAVE_WINDOWS_H
    char buff[_MAX_PATH] {};
    auto len = GetModuleFileNameA(NULL, buff, sizeof(buff));
    if (!len || len >= sizeof(buff))
        return std::string();

    auto str = dos2unix_path(buff);
#else
    auto str = readlink("/proc/self/exe");
#endif
    return extract_dirname(str);
}

std::string cwd()
{
    std::error_code ec;
    return dos2unix_path(fs::current_path(ec).string());
}

char path_separator()
{
#if defined(HAVE_WINDOWS_H)
    return ';';
#else
    return ':';
#endif
}

std::vector<std::string> glob(const std::string& pattern)
{
    std::vector<std::string> filenames;
#ifdef HAVE_GLOB_H
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));

    const auto return_value = glob(pattern.c_str(), GLOB_TILDE, nullptr, &glob_result);
    if (return_value != 0)
    {
        globfree(&glob_result);
        throw std::runtime_error(fmt::format("glob() failed: {}", return_value));
    }

    for (size_t i = 0; i < glob_result.gl_pathc; ++i)
        filenames.emplace_back(glob_result.gl_pathv[i]);

    globfree(&glob_result);
#elif defined(HAVE_WINDOWS_H)
    HANDLE hFind;
    WIN32_FIND_DATAA file;
    hFind = FindFirstFileA(pattern.c_str(), &file);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            filenames.emplace_back(file.cFileName);
        } while (FindNextFileA(hFind, &file));
        FindClose(hFind);
    }
#else
    ignoreparam(pattern);
#endif
    return filenames;
}

}
}
}
