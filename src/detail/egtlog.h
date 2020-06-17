/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_EGTLOG_H
#define EGT_DETAIL_EGTLOG_H

/**
 * @file
 * @brief EGT logger interface.
 */

#include "egt/detail/meta.h"
#include "detail/fmt.h"
#include <chrono>

#define EGTLOG_LEVEL_TRACE 0
#define EGTLOG_LEVEL_DEBUG 1
#define EGTLOG_LEVEL_INFO 2
#define EGTLOG_LEVEL_WARN 3
#define EGTLOG_LEVEL_ERROR 4
#define EGTLOG_LEVEL_OFF 5

#ifndef EGTLOG_ACTIVE_LEVEL
#define EGTLOG_ACTIVE_LEVEL EGTLOG_LEVEL_INFO
#endif

namespace egt
{
inline namespace v1
{
namespace detail
{

/* FOREGROUND */
#define RST "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

constexpr const char* loglevel_name(int level)
{
    constexpr const char* names [] =
    {
#ifdef WIN32
        "trace",
        "debug",
        "info",
        "warn",
        "error",
#else
        KCYN "trace" RST,
        KMAG "debug" RST,
        KGRN "info" RST,
        KYEL "warn" RST,
        KRED "error" RST,
#endif
    };
    return names[level];
}

/// Set the log level.
void loglevel(int level);

/// Get the log level.
int loglevel();

template<typename T>
using basic_string_view_t = fmt::basic_string_view<T>;

using string_view_t = basic_string_view_t<char>;

template<typename... Args>
inline void log(int level, const Args& ... args)
{
    if (loglevel() <= level)
    {
        auto t = std::chrono::steady_clock::now();
        auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(t);
        auto now = ms.time_since_epoch().count();
        auto user = fmt::format(args...);
        fmt::print("{} [{}] {}\n", now, loglevel_name(level), user);
    }
}

constexpr const char* file_name(const char* path)
{
    const char* file = path;
    while (*path)
    {
        if (*path == '/' || *path == '\\')
            file = path + 1;
        path++;
    }
    return file;
}

template<typename... Args>
inline void log(int level, const char* file, int line,
                const std::chrono::time_point<std::chrono::steady_clock>& t,
                const Args& ... args)
{
    if (loglevel() <= level)
    {
        auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(t);
        auto now = ms.time_since_epoch().count();
        auto user = fmt::format(args...);
        fmt::print("{} [{}] {}:{} {}\n", now, loglevel_name(level),
                   file_name(file), line, user);
    }
}

template<typename... Args>
inline void trace(string_view_t fmt, const Args& ... args)
{
    log(EGTLOG_LEVEL_TRACE, fmt, args...);
}

template<typename... Args>
inline void debug(string_view_t fmt, const Args& ... args)
{
    log(EGTLOG_LEVEL_DEBUG, fmt, args...);
}

template<typename... Args>
inline void info(string_view_t fmt, const Args& ... args)
{
    log(EGTLOG_LEVEL_INFO, fmt, args...);
}

template<typename... Args>
inline void warn(string_view_t fmt, const Args& ... args)
{
    log(EGTLOG_LEVEL_WARN, fmt, args...);
}

template<typename... Args>
inline void error(string_view_t fmt, const Args& ... args)
{
    log(EGTLOG_LEVEL_ERROR, fmt, args...);
}

}
}
}

#if EGTLOG_ACTIVE_LEVEL <= EGTLOG_LEVEL_TRACE
#define EGTLOG_TRACE(...) detail::log(EGTLOG_LEVEL_TRACE, __FILE__, __LINE__, std::chrono::steady_clock::now(), __VA_ARGS__)
#else
#define EGTLOG_TRACE(...) (void)0
#endif

#if EGTLOG_ACTIVE_LEVEL <= EGTLOG_LEVEL_DEBUG
#define EGTLOG_DEBUG(...) detail::log(EGTLOG_LEVEL_DEBUG, __FILE__, __LINE__, std::chrono::steady_clock::now(), __VA_ARGS__)
#else
#define EGTLOG_DEBUG(...) (void)0
#endif

#if EGTLOG_ACTIVE_LEVEL <= EGTLOG_LEVEL_INFO
#define EGTLOG_INFO(...) detail::log(EGTLOG_LEVEL_INFO, __FILE__, __LINE__, std::chrono::steady_clock::now(), __VA_ARGS__)
#else
#define EGTLOG_INFO(...) (void)0
#endif

#if EGTLOG_ACTIVE_LEVEL <= EGTLOG_LEVEL_WARN
#define EGTLOG_WARN(...) detail::log(EGTLOG_LEVEL_WARN, __FILE__, __LINE__, std::chrono::steady_clock::now(), __VA_ARGS__)
#else
#define EGTLOG_WARN(...) (void)0
#endif

#if EGTLOG_ACTIVE_LEVEL <= EGTLOG_LEVEL_ERROR
#define EGTLOG_ERROR(...) detail::log(EGTLOG_LEVEL_ERROR, __FILE__, __LINE__, std::chrono::steady_clock::now(), __VA_ARGS__)
#else
#define EGTLOG_ERROR(...) (void)0
#endif

#endif
