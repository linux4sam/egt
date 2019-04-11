/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/filesystem.h"
#include <cstring>
#include <fstream>
#include <glob.h>
#include <iostream>
#include <iterator>
#include <libgen.h>
#include <limits.h>
#include <memory>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

std::string extract_filename(const std::string& path)
{
    const char sep = '/';

    size_t i = path.rfind(sep, path.length());
    if (i != string::npos)
        return (path.substr(i + 1, path.length() - i));

    return string();
}

std::string extract_dirname(const std::string& path)
{
    unique_ptr<char, decltype(std::free)*> p(strdup(path.c_str()), std::free);
    auto dir = ::dirname(p.get());
    return std::string(dir);
}


bool exists(const std::string& path)
{
    struct stat buf;
    return !stat(path.c_str(), &buf);
}

std::vector<char> read_file(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ios::binary);

    file.unsetf(std::ios::skipws);
    std::streampos filesize;

    file.seekg(0, std::ios::end);
    filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> vec;
    vec.reserve(filesize);

    vec.insert(vec.begin(),
               std::istream_iterator<char>(file),
               std::istream_iterator<char>());

    return vec;
}

std::string readlink(const std::string& path)
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
    return extract_dirname(std::string(readlink("/proc/self/exe")));
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
