/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_ERAW_H
#define EGT_SRC_DETAIL_ERAW_H

#include <egt/geometry.h>
#include <egt/surface.h>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Load an ERAW file into a surface.
 *
 * @param[in] filename The path of the SVG file.
 */
Surface load_eraw(const std::string& filename);

/**
 * Load an ERAW file into a surface.
 *
 * @param[in] buf Pointer to the in-memory data.
 * @param[in] len Size of the data.
 */
Surface load_eraw(const unsigned char* buf,
                  size_t len);

}
}
}

#endif
