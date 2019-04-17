/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_STRING_H
#define EGT_DETAIL_STRING_H

#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

std::string replace_all(std::string str, const std::string& from,
                        const std::string& to);

inline std::string truncate(const std::string& str, size_t width,
                            bool ellipsis = true)
{
    if (str.length() > width)
    {
        if (ellipsis)
            return str.substr(0, width) + "...";
        else
            return str.substr(0, width);
    }

    return str;
}

/**
 * @brief Trim delimiters off the right side of a std::string
 */
std::string rtrim(const std::string& source, const std::string& t = " \t\n\r\0\x0B");

/**
 * @brief Trim delimiters off the left side of a std::string
 */
std::string ltrim(const std::string& source, const std::string& t = " \t\n\r\0\x0B");

/**
 * @brief Trim delimiters off both sides of a std::string
 */
std::string trim(const std::string& source, const std::string& t = " \t\n\r\0\x0B");

}
}
}

#endif
