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

#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Extract the filename portion of a path or url.
 * @param[in] path File path or url.
 */
std::string extract_filename(const std::string& path);

/**
 * Extract the directory name from a path.
 * @param[in] path File or directory path.
 */
std::string extract_dirname(const std::string& path);

/**
 * Determine if a file exists.
 * @param[in] path File path.
 */
bool exists(const std::string& path);

/**
 * Read the contents of a file into a vector.
 * @param[in] path File path.
 */
std::vector<char> read_file(const std::string& path);


/**
 * Read the path of a symlink.
 * @param[in] path Symlink path.
 */
std::string readlink(const std::string& path);

/**
 * Get the path to the current executable.
 */
std::string exe_pwd();

}
}
}

#endif
