/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_VIDEO_GSTMETA_H
#define EGT_DETAIL_VIDEO_GSTMETA_H

#include <gst/gst.h>
#include <memory>

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

}
}
}

#endif
