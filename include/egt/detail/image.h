/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_IMAGE_H
#define EGT_DETAIL_IMAGE_H

/**
 * @file
 * @brief Working with loading images.
 */

#include <egt/detail/meta.h>
#include <egt/types.h>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Load an image from memory.
 */
EGT_API shared_cairo_surface_t load_image_from_memory(const unsigned char* data,
        size_t len,
        const std::string& name = {});

/**
 * Load an image from ResourceManager.
 * @see ResourceManager
 */
EGT_API shared_cairo_surface_t load_image_from_resource(const std::string& name);

/**
 * Load an image from the filesystem.
 */
EGT_API shared_cairo_surface_t load_image_from_filesystem(const std::string& path);

/**
 * Load an image from the network.
 */
EGT_API shared_cairo_surface_t load_image_from_network(const std::string& url);

/**
  * Return the mime type string for a file.
  *
  * This uses libmagic to identify the file if available. The default
  * database file will be used.  To override this, you can specify the path
  * to the database file in the MAGIC environment variable.
  */
EGT_API std::string get_mime_type(const std::string& path);

/**
 * Return the mime type string for a buffer.
 *
 * This uses libmagic to identify the buffer if available. The default
 * database file will be used.  To override this, you can specify the path
 * to the database file in the MAGIC environment variable.
 */
EGT_API std::string get_mime_type(const void* buffer, size_t length);

}
}
}

#endif
