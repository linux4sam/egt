/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_RESPATH_H
#define EGT_RESPATH_H

/**
 * @file
 * @brief Working with respaths.
 */

#include <egt/detail/meta.h>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * Add a search path for files.
 *
 * Any actual file path used, for example when loading images, will be searched
 * for relative to any added path here.
 */
EGT_API void add_search_path(const std::string& path);

/**
 * Given a file path, try to find it related to any added search paths.
 *
 * @return The full path to the file if found, or a path to a file that doesn't
 *         exist.
 */
EGT_API std::string resolve_file_path(const std::string& filename);

}
}

#endif
