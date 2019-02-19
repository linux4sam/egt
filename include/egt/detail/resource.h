/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_RESOURCE_H
#define EGT_RESOURCE_H

#include <cairo.h>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Register a resource as raw data.
 *
 * @warning This does not copy the data, so it must remain available.
 */
void register_resource(const char* name, const unsigned char* data, unsigned int len);

/**
 * Unregister a resource.
 */
void unregister_resource(const char* name);

/**
 * Get the length of a resource.
 */
unsigned int read_resource_length(const char* name);

/**
 * Get a pointer to the in-memory resource data.
 */
const unsigned char* read_resource_data(const char* name);

/**
 * Read data from a resource.
 */
bool read_resource(const char* name, unsigned char* data,
                   unsigned int length, unsigned int offset = 0);

/**
 * Helper function specifically for cairo's cairo_image_surface_create_from_png_stream().
 * @warning This operates as an unresetable stream making it single use.
 */
cairo_status_t read_resource_stream(void* closure, unsigned char* data, unsigned int length);

}
}
}

#endif
