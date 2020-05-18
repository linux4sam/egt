/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/string.h"
#include <algorithm>

namespace egt
{
inline namespace v1
{
namespace detail
{

std::string replace_all(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

void strip(std::string& str, const std::string& t)
{
    size_t count = 0;

    for (size_t i = 0; i < str.size(); i++)
        if (t.find(str[i]) == std::string::npos)
            str[count++] = str[i];

    str.resize(count);
}

std::string truncate(const std::string& str, size_t width, bool ellipsis)
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

std::string rtrim(const std::string& source, const std::string& t)
{
    std::string str = source;
    return str.erase(str.find_last_not_of(t) + 1);
}

std::string ltrim(const std::string& source, const std::string& t)
{
    std::string str = source;
    return str.erase(0, source.find_first_not_of(t));
}

std::string trim(const std::string& source, const std::string& t)
{
    if (source.empty())
        return source;

    return ltrim(rtrim(source, t), t);
}

void tokenize(const std::string& str, char delimiter, std::vector<std::string>& tokens)
{
    size_t start = str.find_first_not_of(delimiter);

    while (start != std::string::npos)
    {
        auto end = str.find(delimiter, start);
        tokens.push_back(str.substr(start, end - start));
        start = str.find_first_not_of(delimiter, end);
    }
}

void tolower(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](auto c)
    {
        return std::tolower(c);
    });
}

void toupper(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](auto c)
    {
        return std::toupper(c);
    });
}

bool from_string(const std::string& x)
{
    auto s = x;
    tolower(s);
    return (s == "true" ||
            s == "yes");
}

}
}
}
