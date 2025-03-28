/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/egtlog.h"
#include "egt/detail/image.h"
#include "egt/detail/meta.h"
#include "egt/resource.h"
#include <cstring>
#include <memory>

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif

namespace egt
{
inline namespace v1
{

// NOLINTNEXTLINE(hicpp-special-member-functions, cppcoreguidelines-special-member-functions)
struct ResourceManager::ResourceItem
{
    ResourceItem() = delete;

    ResourceItem(const unsigned char* data, size_t len) noexcept
        : m_data(data),
          m_len(len)
    {}

    explicit ResourceItem(std::vector<unsigned char> data)
        : m_data_copy(std::move(data)),
          m_data(m_data_copy.data()),
          m_len(m_data_copy.size())
    {}

    ResourceItem(const ResourceItem& rhs)
        : m_data_copy(rhs.m_data_copy)
    {
        if (!m_data_copy.empty())
        {
            m_data = m_data_copy.data();
            m_len = m_data_copy.size();
        }
        else
        {
            m_data = rhs.m_data;
            m_len = rhs.m_len;
        }
    }

    ResourceItem& operator=(const ResourceItem&) = delete;
    ResourceItem(ResourceItem&&) = default;
    ResourceItem& operator=(ResourceItem&&) = default;

    inline const unsigned char* data()
    {
        do_inflate();
        return m_data;
    }

    inline size_t len()
    {
        do_inflate();
        return m_len;
    }

    size_t index{0};

private:

    void do_inflate()
    {
#ifdef HAVE_ZLIB
        if (m_inflated)
            return;
        m_inflated = true;

        auto mimetype = detail::get_mime_type(m_data, m_len);
        if (mimetype != "application/gzip")
            return;

        z_stream stream{};
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;
        if (inflateInit2(&stream, 15 + 32) != Z_OK)
        {
            detail::warn("failed to init zlib inflate");
            return;
        }

        stream.next_in = (Bytef*)m_data;
        stream.avail_in = m_len;

        const size_t BUFSIZE = 8 * 1024;
        unsigned char buffer[BUFSIZE];
        int res;
        do
        {
            stream.next_out = reinterpret_cast<Bytef*>(buffer);
            stream.avail_out = BUFSIZE;

            res = inflate(&stream, Z_NO_FLUSH);

            if (m_buf.size() < stream.total_out)
                m_buf.insert(m_buf.end(),
                             buffer,
                             buffer + stream.total_out - m_buf.size());

        } while (res == Z_OK);

        inflateEnd(&stream);

        if (res != Z_STREAM_END)
        {
            detail::warn("failed to finish zlib inflate: {}", res);
            return;
        }

        m_data = m_buf.data();
        m_len = m_buf.size();
        m_data_copy.clear();
#endif
    }

    std::vector<unsigned char> m_data_copy;
    const unsigned char* m_data{nullptr};
    size_t m_len{0};
    std::vector<unsigned char> m_buf;
#ifdef HAVE_ZLIB
    bool m_inflated {false};
#endif
};

ResourceManager::ResourceManager() = default;

ResourceManager& ResourceManager::instance()
{
    static const std::unique_ptr<ResourceManager> i(new ResourceManager());
    return *i;
}

bool ResourceManager::exists(const char* name) const
{
    const auto i = m_resources.find(name);
    return i != m_resources.end();
}

void ResourceManager::clear()
{
    m_resources.clear();
}

void ResourceManager::clear(const char* name)
{
    const auto i = m_resources.find(name);
    if (i != m_resources.end())
        m_resources.erase(i);
}

size_t ResourceManager::size(const char* name)
{
    const auto i = m_resources.find(name);
    if (i != m_resources.end())
        return i->second.len();

    return 0;
}

const unsigned char* ResourceManager::data(const char* name)
{
    const auto i = m_resources.find(name);
    if (i != m_resources.end())
        return i->second.data();

    return nullptr;
}

bool ResourceManager::read(const char* name, unsigned char* data,
                           size_t length, size_t offset)
{
    const auto i = m_resources.find(name);
    if (i != m_resources.end())
    {
        if ((offset + length) > i->second.len())
            throw std::runtime_error("out of bounds read on resource");

        memcpy(data, i->second.data() + offset, length);
        return true;
    }

    return false;
}

void ResourceManager::stream_reset(const char* name)
{
    const auto i = m_resources.find(name);
    if (i != m_resources.end())
        i->second.index = 0;
}

bool ResourceManager::stream_read(const char* name, unsigned char* data,
                                  size_t length)
{
    const auto i = m_resources.find(name);
    if (i != m_resources.end())
    {
        if ((i->second.index + length) > i->second.len())
            throw std::runtime_error("read past end of data on resource");

        memcpy(data, i->second.data() + i->second.index, length);
        i->second.index += length;

        return true;
    }

    return false;
}

template<typename T1, typename T2>
std::vector<T1> extract_keys(std::map<T1, T2> const& input_map)
{
    std::vector<T1> ret;
    ret.reserve(input_map.size());
    for (auto const& element : input_map)
        ret.push_back(element.first);
    return ret;
}

ResourceManager::ItemArray ResourceManager::list() const
{
    return extract_keys(m_resources);
}

void ResourceManager::add(const char* name, const unsigned char* data, size_t len)
{
    if (exists(name))
        detail::warn("resource added with duplicate name: {}", name);

    ResourceItem r(data, len);
    m_resources.insert(std::make_pair(name, r));
}

void ResourceManager::add(const char* name, const std::vector<unsigned char>& data)
{
    if (exists(name))
        detail::warn("resource added with duplicate name: {}", name);

    ResourceItem r(data);
    m_resources.insert(std::make_pair(name, r));
}

void ResourceManager::remove(const char* name)
{
    const auto i = m_resources.find(name);
    if (i != m_resources.end())
        m_resources.erase(i);
}

}
}
