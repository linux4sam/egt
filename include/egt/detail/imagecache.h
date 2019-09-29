/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_IMAGECACHE_H
#define EGT_DETAIL_IMAGECACHE_H

#include <egt/painter.h>
#include <egt/utils.h>
#include <map>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Add a search path for files.
 *
 * Any actual file path used, for example when loading images, will be searched
 * for relative to any added path here.
 */
void add_search_path(const std::string& path);

/**
 * Given a file path, try to find it related to any added search paths.
 *
 * @return The full path to the file if found, or a path to a file that doesn't
 *         exist.
 */
std::string resolve_file_path(const std::string& filename);

/**
 * Internal image cache.
 *
 * Provides an in-memory cache for images based on filename and scale. This
 * prevents multiple attempts at loading the same file as well as re-scaling
 * the image to the same scale multiple times.
 *
 * This is a trade off in consuming more memory instead of possibly
 * constantly reloading or scaling the same image.
 */
class ImageCache : public detail::noncopyable
{
public:

    /**
     * Get an image surface.
     */
    shared_cairo_surface_t get(const std::string& filename,
                               float hscale = 1.0, float vscale = 1.0,
                               bool approximate = true);

    /**
     * Clear the image cache.
     */
    void clear();

    static shared_cairo_surface_t scale_surface(const shared_cairo_surface_t& old_surface,
            float old_width, float old_height,
            float new_width, float new_height);

    /**
     * Return the mime type string for a filename.
     */
    static std::string get_mime_type(const std::string& filename);

    /**
     * Return the mime type string for a buffer.
     */
    static std::string get_mime_type(const void* buffer, size_t length);

protected:

    static float round(float v, float fraction);

    std::string id(const std::string& filename, float hscale, float vscale);

    std::map<std::string, shared_cairo_surface_t> m_cache;
};

/**
 * Global image cache instance.
 */
ImageCache& image_cache();

}
}
}

#endif
