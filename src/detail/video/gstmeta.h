/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_VIDEO_GSTMETA_H
#define EGT_SRC_DETAIL_VIDEO_GSTMETA_H

#include "egt/detail/filesystem.h"
#include <gst/gst.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

template<class T, void Func(T*)>
struct GstDeleter
{
    inline void operator()(void* p) const
    {
        if (p)
            Func(static_cast<T*>(p));
    }
};

using GstStringHandle = std::unique_ptr<gchar, GstDeleter<void, g_free>>;
using GstErrorHandle = std::unique_ptr<GError, GstDeleter<GError, g_error_free>>;

template<class T>
inline void gst_message_parse(T& func, GstMessage* msg, GstErrorHandle& err, GstStringHandle& debug)
{
    GError* error = nullptr;
    gchar* string = nullptr;
    func(msg, &error, &string);
    err.reset(error);
    debug.reset(string);
}

template<class T>
inline void gst_init_plugins(T& plugins)
{
    GError* err = nullptr;
    if (!gst_init_check(nullptr, nullptr, &err))
    {
        std::string ss = "failed to initialize gstreamer: ";
        if (err && err->message)
        {
            ss = ss + err->message;
            g_error_free(err);
        }
        else
        {
            ss = ss + "unknown error";
        }
        throw std::runtime_error(ss);
    }

    /**
      * check for cache file by finding a playback plugin.
      * if gst_registry_find_plugin returns NULL, then no
      * cache file present and assume GSTREAMER1_PLUGIN_REGISTRY
      * is disabled in gstreamer package.
      */
    if (!gst_registry_find_plugin(gst_registry_get(), "playback"))
    {
        EGTLOG_DEBUG("manually loading gstreamer plugins");

        std::string path;
        if (std::getenv("GST_PLUGIN_SYSTEM_PATH"))
        {
            path = std::getenv("GST_PLUGIN_SYSTEM_PATH");
            if (!path.empty() && (path.back() != '/'))
                path.back() = '/';
        }
        else if (detail::exists("/usr/lib/gstreamer-1.0/"))
        {
            path = "/usr/lib/gstreamer-1.0/";
        }
        else if (detail::exists("/usr/lib/x86_64-linux-gnu/gstreamer-1.0/"))
        {
            path = "/usr/lib/x86_64-linux-gnu/gstreamer-1.0/";
        }

        for (const auto& plugin : plugins)
        {
            GError* error = nullptr;
            gst_plugin_load_file(std::string(path + plugin).c_str(), &error);
            if (error)
            {
                if (error->message)
                    detail::error("load plugin error: {}", error->message);
                g_error_free(error);
            }
        }
    }
}

}
}
}

#endif
