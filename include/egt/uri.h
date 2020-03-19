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
 * RFC 3986 compliant.
 *
 * The current implementation uses the regex recommended by
 * https://tools.ietf.org/html/rfc3986#page-50
 */
class EGT_API Uri
{
public:

    Uri() = default;

    /**
     * Construct a URI from a string
     *
     * @param[in] uri URI string
     */
    // cppcheck-suppress noExplicitConstructor
    Uri(const std::string& uri);

    /// Get the scheme part of the URI
    std::string scheme() const { return m_scheme; }
    /// Get the host part of the URI
    std::string host() const { return m_host; }
    /// Get the port poart of the URI
    std::string port() const { return m_port; }
    /// Get the path part of the URI
    std::string path() const { return m_path; }
    /// Get the query part of the URI
    std::string query() const { return m_query; }
    /// Get the fragment part of the URI
    std::string fragment() const { return m_fragment; }
    /// Get the icon size of the URI
    std::string icon_size() const { return m_icon_size; }

    /// Is the URI syntactically valid.
    bool is_valid() const;

    /// Convert the URI to a string
    std::string to_string() const;

    /// Convert the URI to a string
    operator std::string() const
    {
        return to_string();
    }

    /// Set the URI from a string.
    void set(const std::string& uri)
    {
        parse(uri);
    }

private:

    void clear();
    void parse(const std::string& uri);

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
