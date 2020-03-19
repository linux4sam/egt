/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_RESOURCE_H
#define EGT_RESOURCE_H

/**
 * @file
 * @brief Working with resources.
 */

#include <cairo.h>
#include <cstdint>
#include <egt/detail/meta.h>
#include <map>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * Manages EGT resource data blobs.
 *
 * Resources can be registered with this class and then read back later directly
 * from this class.  When using an EGT API, like Image() that takes a URI,
 * prefix the name in the URI with scheme 'res' to make EGT read the resource
 * from ResourceManager.
 *
 * @see @ref resources
 */
class EGT_API ResourceManager : private detail::NonCopyable<ResourceManager>
{
public:

    /**
     * Get a reference to the ResourceManager instance.
     */
    static ResourceManager& instance();

    /**
     * @return true if the resource with the given name is registered.
     */
    bool exists(const char* name) const;

    /**
     * Register a resource as raw data.
     *
     * @warning This does not copy the data, so it must remain available.
     */
    void add(const char* name, const unsigned char* data, size_t len);

    /**
     * Unregister a resource.
     */
    void remove(const char* name);

    /**
     * Clear all registered resources.
     */
    void clear();

    /**
     * Clear the specified resource by name.
     */
    void clear(const char* name);

    /**
     * Get the data length of a resource.
     */
    size_t size(const char* name);

    /**
     * Get a pointer to the in-memory resource data.
     */
    const unsigned char* data(const char* name);

    /**
     * Read data from a resource.
     */
    bool read(const char* name, unsigned char* data,
              size_t length, size_t offset = 0);

    /// Item array type.
    using ItemArray = std::vector<std::string>;

    /**
     * Get a list of registered resource names.
     */
    ItemArray list() const;

    /**
     * Reset internal read stream offset.
     *
     * @see read_stream()
     */
    void stream_reset(const char* name);

    /**
     * Read starting from internal stream offset.
     */
    bool stream_read(const char* name, unsigned char* data, size_t length);

private:

    ResourceManager();

    struct ResourceItem;

    using ResourceMap = std::map<std::string, ResourceItem>;

    ResourceMap m_resources;
};

namespace detail
{
/**
 * Helper function specifically for cairo's cairo_image_surface_create_from_png_stream().
 */
cairo_status_t EGT_API read_resource_stream(void* closure, unsigned char* data,
        unsigned int length);
}

}
}

#endif
