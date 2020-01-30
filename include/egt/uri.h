/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_URI_H
#define EGT_URI_H

/**
 * @file
 * @brief Working with URIs.
 */

#include <egt/detail/meta.h>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * A URI (Universal Resource Identifier) parser/builder that aims to be
 * RFC 3986 compliant, yet knowlingly falls short.
 *
 * The current implementation uses the regex recommended by
 * https://tools.ietf.org/html/rfc3986#page-50
 */
class Uri
{
public:

    Uri() = default;

    // cppcheck-suppress noExplicitConstructor
    Uri(const std::string& url);

    inline std::string scheme() const { return m_scheme; }
    inline std::string host() const { return m_host; }
    inline std::string port() const { return m_port; }
    inline std::string path() const { return m_path; }
    inline std::string query() const { return m_query; }
    inline std::string fragment() const { return m_fragment; }
    inline std::string icon_size() const { return m_icon_size; }

    bool is_valid() const;

    std::string to_string() const;

    operator std::string() const
    {
        return to_string();
    }

    inline void set(const std::string& url)
    {
        parse(url);
    }

private:

    void clear();
    void parse(const std::string& url);

    std::string m_scheme;
    std::string m_host;
    std::string m_port;
    std::string m_path;
    std::string m_query;
    std::string m_fragment;
    std::string m_icon_size;
};

}
}

#endif
