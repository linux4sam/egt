/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/string.h"
#include "egt/uri.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <string>

namespace egt
{
inline namespace v1
{

Uri::Uri(const std::string& uri)
{
    parse(uri);
}

bool Uri::is_valid() const
{
    return !m_scheme.empty() &&
           (!m_host.empty() || !m_path.empty());
}

std::string Uri::to_string() const
{
    return m_scheme + ":" +
           (!m_host.empty() ? "//" + m_host : "") +
           (!m_host.empty() && !m_port.empty() ? ":" + m_port : "") +
           m_path +
           m_query +
           (!m_fragment.empty() ? "#" + m_fragment : "");
}

void Uri::clear()
{
    m_scheme.clear();
    m_host.clear();
    m_port.clear();
    m_path.clear();
    m_query.clear();
    m_fragment.clear();
    m_icon_size.clear();
}

// Keep an eye on http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3420.html
void Uri::parse(const std::string& uri)
{
    clear();

    // regex suggested by RFC 3986: https://tools.ietf.org/html/rfc3986#page-50
    static const std::regex uri_regex(
        R"(^(([^:\/?#]+):)?(//([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)",
        std::regex::extended
    );

    std::smatch match;
    if (std::regex_match(uri, match, uri_regex))
    {
#ifndef NDEBUG
        int counter = 0;
        for (const auto& res : match)
        {
            SPDLOG_TRACE("{}: {}", counter++, res);
        }
#endif

        if (match.size() >= 3)
        {
            m_scheme = match.str(2);
            detail::tolower(m_scheme);
        }
        if (match.size() >= 5)
        {
            m_host = match.str(4);

            std::vector<std::string> tokens;
            detail::tokenize(m_host, ':', tokens);
            if (tokens.size() == 2)
            {
                m_host = tokens[0];
                m_port = tokens[1];
            }

            detail::tolower(m_host);
        }
        if (match.size() >= 6)
        {
            m_path = match.str(5);

            // https://tools.ietf.org/pdf/draft-lafayette-icon-uri-scheme-01.pdf
            if (m_scheme == "icon")
            {
                std::vector<std::string> tokens;
                detail::tokenize(m_path, ';', tokens);
                if (tokens.size() == 2)
                {
                    m_path = tokens[0];
                    m_icon_size = tokens[1];

                    detail::tolower(m_icon_size);

                    if (m_icon_size == "small")
                        m_icon_size = "16";
                    else if (m_icon_size == "medium")
                        m_icon_size = "64";
                    else if (m_icon_size == "large")
                        m_icon_size = "256";
                }
            }
            else if (m_scheme == "file")
            {
                m_path = m_host + m_path;
                m_host.clear();
            }
        }
        if (match.size() >= 8)
            m_query = match.str(7);
        if (match.size() >= 10)
            m_fragment = match.str(9);
    }
    else
    {
        spdlog::warn("invalid uri: {}", uri);
        clear();
    }
}

}
}
