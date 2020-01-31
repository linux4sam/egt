/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_STRING_H
#define EGT_DETAIL_STRING_H

/**
 * @file
 * @brief String utilities.
 */

#include <egt/detail/meta.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace detail
{

EGT_API std::string replace_all(std::string str, const std::string& from,
                                const std::string& to);

EGT_API inline std::string truncate(const std::string& str, size_t width,
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
 * Trim delimiters off the right side of a std::string
 */
EGT_API std::string rtrim(const std::string& source, const std::string& t = " \t\n\r");

/**
 * Trim delimiters off the left side of a std::string
 */
EGT_API std::string ltrim(const std::string& source, const std::string& t = " \t\n\r");

/**
 * Trim delimiters off both sides of a std::string
 */
EGT_API std::string trim(const std::string& source, const std::string& t = " \t\n\r");

/**
 * Format a float/double to a fixed precision and return as a string.
 */
template<class T>
std::string format(T value, int precision = 2)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

/**
 * Tokenize a std::string
 */
EGT_API void tokenize(const std::string& str, char delimiter, std::vector<std::string>& tokens);

/**
 * Join each item of a container with the specified delimiter between each item.
 */
template<class T>
void join(std::ostream& os, const T& container, const std::string& delimiter = ",")
{
    bool first = true;
    for (auto& item : container)
    {
        if (first)
            first = false;
        else
            os << delimiter;
        os << item;
    }
}

/**
 * Convert a type to a std::string using std::ostringstream.
 */
template<class T>
inline std::string to_string(const T& x)
{
    std::ostringstream ss;
    ss << x;
    return ss.str();
}

}
}
}

#endif
