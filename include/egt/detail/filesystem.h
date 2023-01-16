/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_FILESYSTEM_H
#define EGT_DETAIL_FILESYSTEM_H

/**
 * @file
 * @brief Filesystem utilities.
 */

#include <egt/detail/meta.h>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Extract the filename portion of a path or URL.
 *
 * @param[in] path File path or URL.
 */
EGT_API std::string extract_filename(const std::string& path);

/**
 * Extract the directory name from a path.
 *
 * @param[in] path File or directory path.
 */
EGT_API std::string extract_dirname(const std::string& path);

/**
 * Determine if a file exists.
 *
 * @param[in] path File path.
 */
EGT_API bool exists(const std::string& path);

/**
 * Read the contents of a file into a vector.
 *
 * @param[in] path File path.
 */
EGT_API std::vector<unsigned char> read_file(const std::string& path);

/**
 * Read the path of a symlink.
 *
 * @param[in] path Symlink path.
 */
EGT_API std::string readlink(const std::string& path);

/**
 * Convert a relative path to an absolute path.
 *
 * This is similar to POSIX.1-2001 realpath().
 */
EGT_API std::string abspath(const std::string& path);

/**
 * Get the directory to the current executable.
 */
EGT_API std::string exe_pwd();

/**
 * Get the current working directory.
 */
EGT_API std::string cwd();

/**
 * Get the separator character in a list of paths.
 *
 * Used, for instance, in EGT_SEARCH_PATH. It should be ':' on Linux and
 * ';' on Windows.
 */
EGT_API char path_separator();

/**
 * Given a glob pattern, return a vector of matching path names.
 */
EGT_API std::vector<std::string> glob(const std::string& pattern);

}
}
}

#endif
