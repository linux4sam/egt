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


}
}
}

#endif
