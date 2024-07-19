/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SRC_DETAIL_VIDEO_GSTMETA_H
#define EGT_SRC_DETAIL_VIDEO_GSTMETA_H

#include "egt/detail/filesystem.h"
#include "detail/egtlog.h"
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
using GstStructureHandle = std::unique_ptr<GstStructure, GstDeleter<GstStructure, gst_structure_free >>;

template<class T>
inline void gstreamer_message_parse(T& func, GstMessage* msg, GstErrorHandle& err, GstStringHandle& debug)
{
    GError* gst_error = nullptr;
    gchar* string = nullptr;
    func(msg, &gst_error, &string);
    err.reset(gst_error);
    debug.reset(string);
}

void gstreamer_init();

std::string gstreamer_get_device_path(GstDevice* device);

}
}
}

#endif
