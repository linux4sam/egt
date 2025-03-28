/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_IMAGECACHE_H
#define EGT_DETAIL_IMAGECACHE_H

#include <egt/detail/meta.h>
#include <egt/painter.h>
#include <egt/surface.h>
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
 * Internal image cache.
 *
 * Provides an in-memory cache for images based on name and scale. This
 * prevents multiple attempts at loading the same file as well as re-scaling
 * the image to the same scale multiple times.
 *
 * This is a trade off in consuming more memory instead of possibly
 * constantly reloading or scaling the same image.
 */
class EGT_API ImageCache
{
public:

    /**
     * Get an image surface.
     */
    std::shared_ptr<Surface> get(const std::string& uri,
                                 float hscale = 1.0, float vscale = 1.0,
                                 bool approximate = true);

    /**
     * Clear the image cache.
     */
    void clear();

protected:

    static Surface resize(const Surface& surface, const Size& size);

    static float round(float v, float fraction);

    static std::string id(const std::string& name, float hscale, float vscale);

    std::map<std::string, std::shared_ptr<Surface>> m_cache;
};

/**
 * Global image cache instance.
 */
EGT_API ImageCache& image_cache();

}
}
}

#endif
