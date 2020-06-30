/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_BASE64_H
#define EGT_SRC_DETAIL_BASE64_H

#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Base64 encode
 *
 * @param src Data to be encoded
 * @param len Length of the data to be encoded
 *
 * @return Allocated buffer of encoded data or empty string on failure.
 */
std::string base64_encode(const unsigned char* src, size_t len);

std::string base64_decode(const void* data, size_t len);

}
}
}

#endif
