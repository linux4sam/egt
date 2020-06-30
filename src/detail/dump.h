/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_DUMP_H
#define EGT_SRC_DETAIL_DUMP_H

/**
 * @file
 * @brief Debug dump tools.
 */

#include "detail/fmt.h"
#include <chrono>
#include <iomanip>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace detail
{

template<class Prefix, class T>
inline void code_timer(bool enable, const Prefix& prefix, T&& callback)
{
    if (enable)
    {
        const auto start = std::chrono::steady_clock::now();

        callback();

        const auto end = std::chrono::steady_clock::now();
        const auto diff = end - start;

        fmt::print("{} {}\n", prefix,
                   std::chrono::duration<double, std::milli>(diff).count());
    }
    else
    {
        callback();
    }
}

/**
 * Utility to print a somewhat standard hex display.
 */
template<typename T = std::uint8_t>
std::ostream & hex_dump(std::ostream& out, const T* buffer, std::size_t size)
{
    constexpr auto ptr_size = sizeof(void*);
    constexpr auto byte_size = sizeof(T);
    constexpr auto words_per_row = 4;

    const auto bytes = std::vector<T>(buffer, buffer + size);
    for (auto p = bytes.cbegin(); p != bytes.cend();)
    {
        const auto bytes_to_go =
            static_cast<unsigned>(std::distance(p, cend(bytes)));
        const auto num_bytes = std::min<std::size_t>(ptr_size, bytes_to_go);
        const auto bytes_printed =
            static_cast<unsigned>(std::distance(cbegin(bytes), p));
        const auto first_row = bytes_printed == 0;
        const auto new_row = (bytes_printed % (ptr_size * words_per_row)) == 0;
        const auto last_row = (p + num_bytes) == cend(bytes);

        if (new_row && !last_row)
        {
            if (!first_row)
                out << '\n';

            out << std::hex << std::setw(ptr_size * 2) << std::setfill('0') <<
                bytes_printed << "   ";
        }

        for_each(p, p + num_bytes, [&out](T byte)
        {
            out << ' ' << std::hex << std::setw(byte_size * 2) <<
                std::setfill('0') << static_cast<unsigned>(byte);
        });
        out << "   ";

        if (last_row)
            out << '\n';

        p += num_bytes;
    }

    return out;
}

}
}
}

#endif
