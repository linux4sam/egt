/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_ERAWIMAGE_H
#define EGT_SRC_DETAIL_ERAWIMAGE_H

#include <cairo.h>
#include <cstring>
#include <egt/geometry.h>
#include <egt/types.h>
#include <fstream>
#include <string>

extern "C" {
    extern void* arm_memset32(uint32_t*, uint32_t, size_t);
}

namespace egt
{
inline namespace v1
{
namespace detail
{

enum class endian
{
#ifdef WIN32
    little = 0,
    big    = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};

static_assert(endian::native == endian::little,
              "eraw implementation only works on little");

class ErawImage
{
private:

#ifdef __arm__
    static inline void memset32(uint32_t* data, uint32_t value, size_t count)
    {
        arm_memset32(data, value, count);
    }
#else
    static void memset32(uint32_t* data, uint32_t value, size_t count)
    {
        const union
        {
            uint32_t v;
            uint8_t c[sizeof(uint32_t)];
        } u = {value};

        if (u.c[0] == u.c[1] &&
            u.c[1] == u.c[2] &&
            u.c[2] == u.c[3])
        {
            memset(data, u.c[0], count * sizeof(uint32_t));
        }
        else
        {
            while (count--)
                *data++ = value;
        }
    }
#endif

    template <class T>
    static const uint8_t* readw(const uint8_t* data, T& value, const uint8_t* end)
    {
        static_assert(std::is_integral<T>::value, "T must be an integer!");
        if (data + sizeof(T) > end)
            return nullptr;
        memcpy(&value, data, sizeof(T));
        return data + sizeof(T);
    }

public:

    static constexpr uint32_t egt_magic()
    {
        return 0x50502AA2;
    }

    static shared_cairo_surface_t load(const std::string& filename)
    {
        std::ifstream i(filename, std::ios_base::binary);
        if (!i)
            return nullptr;
        alignas(4) uint32_t magic = 0;
        alignas(4) uint32_t width = 0;
        alignas(4) uint32_t height = 0;
        alignas(4) uint32_t reserved = 0;
        i.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        if (magic != egt_magic())
            return nullptr;
        if (!i.read(reinterpret_cast<char*>(&width), sizeof(width)) ||
            !i.read(reinterpret_cast<char*>(&height), sizeof(height)) ||
            !i.read(reinterpret_cast<char*>(&reserved), sizeof(reserved)) ||
            !i.read(reinterpret_cast<char*>(&reserved), sizeof(reserved)) ||
            !i.read(reinterpret_cast<char*>(&reserved), sizeof(reserved)) ||
            !i.read(reinterpret_cast<char*>(&reserved), sizeof(reserved)))
            return nullptr;

        auto surface =
            shared_cairo_surface_t(cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                   width, height),
                                   cairo_surface_destroy);
        auto data =
            reinterpret_cast<uint32_t*>(cairo_image_surface_get_data(surface.get()));
        auto end = data + (width * height);

        while (data < end)
        {
            alignas(4) uint16_t block = 0;
            if (!i.read(reinterpret_cast<char*>(&block), sizeof(block)))
                return nullptr;
            if (block & 0x8000)
            {
                block &= 0x7fff;
                alignas(4) uint32_t value = 0;
                if (!i.read(reinterpret_cast<char*>(&value), sizeof(value)))
                    return nullptr;
                memset32(data, value, block);
            }
            else if (block)
            {
                if (!i.read(reinterpret_cast<char*>(data), block * sizeof(uint32_t)))
                    return nullptr;
            }
            data += block;
        }

        i.close();

        // must mark surface dirty once we manually fill it in
        cairo_surface_mark_dirty(surface.get());

        return surface;
    }

    static shared_cairo_surface_t read_surface_data(const unsigned char* buf, const unsigned char* buf_end, uint32_t width, uint32_t height)
    {
        auto surface =
            shared_cairo_surface_t(cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                   width, height),
                                   cairo_surface_destroy);
        auto data =
            reinterpret_cast<uint32_t*>(cairo_image_surface_get_data(surface.get()));
        auto end = data + (width * height);

        while (data < end)
        {
            alignas(4) uint16_t block = 0;
            buf = readw(buf, block, buf_end);
            if (!buf)
                return nullptr;
            if (block & 0x8000)
            {
                block &= 0x7fff;
                alignas(4) uint32_t value = 0;
                buf = readw(buf, value, buf_end);
                if (!buf)
                    return nullptr;
                memset32(data, value, block);
            }
            else if (block)
            {
                if (buf + block * sizeof(uint32_t) > buf_end)
                    return nullptr;

                memcpy(data, buf, block * sizeof(uint32_t));
                buf += (block * sizeof(uint32_t));
            }
            data += block;
        }

        // must mark surface dirty once we manually fill it in
        cairo_surface_mark_dirty(surface.get());

        return surface;
    }

    static shared_cairo_surface_t load(const unsigned char* buf, size_t len)
    {
        const auto buf_end = buf + len;
        alignas(4) uint32_t magic = 0;
        alignas(4) uint32_t width = 0;
        alignas(4) uint32_t height = 0;

        buf = readw(buf, magic, buf_end);
        if (!buf)
            return nullptr;
        if (magic != egt_magic())
            return nullptr;
        buf = readw(buf, width, buf_end);
        if (!buf)
            return nullptr;
        buf = readw(buf, height, buf_end);
        if (!buf)
            return nullptr;
        buf += (sizeof(uint32_t) * 4);

        return read_surface_data(buf, buf_end, width, height);
    }

    static shared_cairo_surface_t load(const unsigned char* buf, size_t len, std::shared_ptr<Rect>& rect)
    {
        const auto buf_end = buf + len;
        alignas(4) uint32_t magic = 0;
        alignas(4) uint32_t width = 0;
        alignas(4) uint32_t height = 0;
        alignas(4) int32_t x = 0;
        alignas(4) int32_t y = 0;

        buf = readw(buf, magic, buf_end);
        if (!buf)
            return nullptr;
        if (magic != egt_magic())
            return nullptr;
        buf = readw(buf, width, buf_end);
        if (!buf)
            return nullptr;
        buf = readw(buf, height, buf_end);
        if (!buf)
            return nullptr;
        buf = readw(buf, x, buf_end);
        if (!buf)
            return nullptr;
        buf = readw(buf, y, buf_end);
        if (!buf)
            return nullptr;
        buf += (sizeof(uint32_t) * 2);

        rect->x(x);
        rect->y(y);
        rect->width(width);
        rect->height(height);

        return read_surface_data(buf, buf_end, width, height);
    }

    static uint16_t next_diff_block(uint32_t* data, const uint32_t* end)
    {
        if (end - data > 0x7fff)
            end = data + 0x7fff;

        auto ptr = data + 1;
        while (ptr < end)
        {
            if (*ptr == *(ptr - 1))
                break;

            ptr++;
        }
        return ptr - data;
    }

    static uint16_t next_same_block(uint32_t* data, const uint32_t* end, uint32_t& value)
    {
        if (end - data > 0x7fff)
            end = data + 0x7fff;

        value = *data;
        auto ptr = data;
        while (ptr < end && *ptr == value)
            ptr++;
        if (ptr - data >= 2)
            return ptr - data;
        return 0;
    }

    static void save(const std::string& path, unsigned char* data, uint32_t width, uint32_t height)
    {
        std::ofstream o(path, std::ios_base::binary);
        const auto magic = egt_magic();
        o.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        o.write(reinterpret_cast<const char*>(&width), sizeof(width));
        o.write(reinterpret_cast<const char*>(&height), sizeof(height));
        uint32_t reserved = 0;
        o.write(reinterpret_cast<const char*>(&reserved), sizeof(reserved));
        o.write(reinterpret_cast<const char*>(&reserved), sizeof(reserved));
        o.write(reinterpret_cast<const char*>(&reserved), sizeof(reserved));
        o.write(reinterpret_cast<const char*>(&reserved), sizeof(reserved));

        const auto start = reinterpret_cast<uint32_t*>(data);
        auto offset = reinterpret_cast<uint32_t*>(data);
        const auto end = reinterpret_cast<uint32_t*>(start + (width * height));

        while (offset < end)
        {
            uint32_t value = 0;
            auto same = next_same_block(offset, end, value);
            if (same)
            {
                offset += same;
                same |= 0x8000;
                o.write(reinterpret_cast<const char*>(&same), sizeof(same));
                o.write(reinterpret_cast<const char*>(&value), sizeof(value));
            }
            else
            {
                auto diff = next_diff_block(offset, end);
                if (diff)
                {
                    o.write(reinterpret_cast<const char*>(&diff), sizeof(diff));
                    o.write(reinterpret_cast<const char*>(offset), diff * sizeof(uint32_t));
                    offset += diff;
                }
            }
        }
        o.close();
    }

    static void save(const std::string& path, unsigned char* data, int32_t x, int32_t y, uint32_t width, uint32_t height, uint32_t* len)
    {
        uint32_t length = 0;
        std::ofstream o(path, std::ios_base::app | std::ios_base::binary);
        const auto magic = egt_magic();
        o.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        length += sizeof(magic);
        o.write(reinterpret_cast<const char*>(&width), sizeof(width));
        length += sizeof(width);
        o.write(reinterpret_cast<const char*>(&height), sizeof(height));
        length += sizeof(height);

        //add x,y coordinate
        o.write(reinterpret_cast<const char*>(&x), sizeof(x));
        length += sizeof(x);
        o.write(reinterpret_cast<const char*>(&y), sizeof(y));
        length += sizeof(y);

        uint32_t reserved = 0;
        o.write(reinterpret_cast<const char*>(&reserved), sizeof(reserved));
        o.write(reinterpret_cast<const char*>(&reserved), sizeof(reserved));
        length += (2 * sizeof(reserved));

        const auto start = reinterpret_cast<uint32_t*>(data);
        auto offset = reinterpret_cast<uint32_t*>(data);
        const auto end = reinterpret_cast<uint32_t*>(start + (width * height));

        while (offset < end)
        {
            uint32_t value = 0;
            auto same = next_same_block(offset, end, value);
            if (same)
            {
                offset += same;
                same |= 0x8000;
                o.write(reinterpret_cast<const char*>(&same), sizeof(same));
                length += sizeof(same);
                o.write(reinterpret_cast<const char*>(&value), sizeof(value));
                length += sizeof(value);
            }
            else
            {
                auto diff = next_diff_block(offset, end);
                if (diff)
                {
                    o.write(reinterpret_cast<const char*>(&diff), sizeof(diff));
                    length += sizeof(diff);
                    o.write(reinterpret_cast<const char*>(offset), diff * sizeof(uint32_t));
                    length += diff * sizeof(uint32_t);
                    offset += diff;
                }
            }
        }
        o.close();

        *len = length;
    }

};

}
}
}

#endif
