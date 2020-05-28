/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/app.h"
#include "egt/detail/filesystem.h"
#include "egt/detail/image.h"
#include "egt/resource.h"
#include "images/bmp/cairo_bmp.h"
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <vector>

#ifdef HAVE_LIBCURL
#include "egt/network/http.h"
#endif

#ifdef HAVE_LIBMAGIC
#include <magic.h>
#endif

#ifdef HAVE_LIBJPEG
#ifdef __cplusplus
extern "C" {
#endif
#include "images/jpeg/cairo_jpg.h"
#ifdef __cplusplus
}
#endif
#endif

#ifdef HAVE_LIBRSVG
#include "detail/svg.h"
#endif

namespace egt
{
inline namespace v1
{
namespace detail
{

struct StreamObject
{
    const unsigned char* data{nullptr};
    size_t len{0};
    size_t offset{0};
};

static cairo_status_t read_stream(void* closure, unsigned char* data, unsigned int length)
{
    auto stream = static_cast<StreamObject*>(closure);

    if ((stream->offset + length) > stream->len)
        throw std::runtime_error("read past end of data stream");

    memcpy(data, stream->data + stream->offset, length);
    stream->offset += length;

    return CAIRO_STATUS_SUCCESS;
}

static constexpr auto MIME_BMP = "image/x-ms-bmp";
#ifdef HAVE_LIBJPEG
static constexpr auto MIME_JPEG = "image/jpeg";
#endif
#if CAIRO_HAS_PNG_FUNCTIONS == 1
static constexpr auto MIME_PNG = "image/png";
#endif
#ifdef HAVE_LIBRSVG
static constexpr auto MIME_SVGXML = "image/svg+xml";
static constexpr auto MIME_SVG = "image/svg";
#endif

EGT_API shared_cairo_surface_t load_image_from_memory(const unsigned char* data,
        size_t len,
        const std::string& name)
{
    if (!data || len <= 0)
        return {};

    shared_cairo_surface_t image;

    const auto mimetype = get_mime_type(data, len);
    SPDLOG_DEBUG("mimetype of {} is {}", name, mimetype);

    if (mimetype == MIME_BMP)
    {
        StreamObject stream = {data, len, 0};
        image = shared_cairo_surface_t(
                    cairo_image_surface_create_from_bmp_stream(
                        read_stream, &stream),
                    cairo_surface_destroy);
    }
#ifdef HAVE_LIBJPEG
    else if (mimetype == MIME_JPEG)
    {
        StreamObject stream = {data, len, 0};
        image = shared_cairo_surface_t(
                    cairo_image_surface_create_from_jpeg_stream(
                        read_stream, &stream),
                    cairo_surface_destroy);
    }
#endif
#if CAIRO_HAS_PNG_FUNCTIONS == 1
    else if (mimetype == MIME_PNG)
    {
        StreamObject stream = {data, len, 0};
        image = shared_cairo_surface_t(
                    cairo_image_surface_create_from_png_stream(
                        read_stream, &stream),
                    cairo_surface_destroy);
    }
#endif
#ifdef HAVE_LIBRSVG
    else if (mimetype == MIME_SVGXML || mimetype == MIME_SVG)
    {
        image = load_svg(data, len);
    }
#endif
    else
    {
        throw std::runtime_error("unsupported image mimetype " + mimetype + " for: " + name);
    }

    return image;
}

shared_cairo_surface_t load_image_from_resource(const std::string& name)
{
    if (!ResourceManager::instance().exists(name.c_str()))
        throw std::runtime_error("resource not found: " + name);

    ResourceManager::instance().stream_reset(name.c_str());

    return load_image_from_memory(ResourceManager::instance().data(name.c_str()),
                                  ResourceManager::instance().size(name.c_str()),
                                  name);
}

shared_cairo_surface_t load_image_from_filesystem(const std::string& path)
{
    if (!detail::exists(path))
        throw std::runtime_error("file not found: " + path);

    const auto mimetype = get_mime_type(path);
    SPDLOG_DEBUG("mimetype of {} is {}", path, mimetype);

    shared_cairo_surface_t image;

    if (mimetype == MIME_BMP)
    {
        image = shared_cairo_surface_t(
                    cairo_image_surface_create_from_bmp(path.c_str()),
                    cairo_surface_destroy);
    }
#ifdef HAVE_LIBJPEG
    else if (mimetype == MIME_JPEG)
    {
        image = shared_cairo_surface_t(
                    cairo_image_surface_create_from_jpeg(path.c_str()),
                    cairo_surface_destroy);
    }
#endif
#if CAIRO_HAS_PNG_FUNCTIONS == 1
    else if (mimetype == MIME_PNG)
    {
        image = shared_cairo_surface_t(
                    cairo_image_surface_create_from_png(path.c_str()),
                    cairo_surface_destroy);
    }
#endif
#ifdef HAVE_LIBRSVG
    else if (mimetype == MIME_SVGXML || mimetype == MIME_SVG)
    {
        image = load_svg(path);
    }
#endif
    else
    {
        throw std::runtime_error("unsupported image mimetype " + mimetype + " for: " + path);
    }

    return image;
}

EGT_API shared_cairo_surface_t load_image_from_network(const std::string& url)
{
    shared_cairo_surface_t image;

#ifdef HAVE_LIBCURL
    auto buffer = experimental::load_file_from_network<std::vector<unsigned char>>(url);

    if (!buffer.empty())
        image = load_image_from_memory(reinterpret_cast<const unsigned char*>(buffer.data()),
                                       buffer.size(),
                                       url);
#else
    detail::ignoreparam(url);
    spdlog::warn("network support not available");
#endif
    return image;
}

#ifdef HAVE_LIBMAGIC
/*
 * There is a known memory leak in magic_load() that may or may not be fixed:
 *    https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=840754
 *
 * The workaround used here is to explicitly specify the default database file
 * instead of using nullptr which makes magic chose the default file internally.
 */
static const auto MAGIC_DATABASE = "/usr/share/misc/magic";
#endif

std::string get_mime_type(const void* buffer, size_t length)
{
    if (!buffer || length <= 0)
        return {};

    std::string result;
#ifdef HAVE_LIBMAGIC
    magic_t magic = magic_open(MAGIC_MIME_TYPE);

    if (magic)
    {
        if (!magic_load(magic, MAGIC_DATABASE))
        {
            auto mime = magic_buffer(magic, buffer, length);
            if (mime)
                result = mime;
        }

        magic_close(magic);
    }
#endif
    return result;
}

std::string get_mime_type(const std::string& path)
{
    std::string result;
#ifdef HAVE_LIBMAGIC
    magic_t magic = magic_open(MAGIC_MIME_TYPE);

    if (magic)
    {
        if (!magic_load(magic, MAGIC_DATABASE))
        {
            auto mime = magic_file(magic, path.c_str());
            if (mime)
                result = mime;
        }

        magic_close(magic);
    }
#else
    if (path.find("png") != std::string::npos)
        result = "image/png";
    else if (path.find("jpg") != std::string::npos)
        result = "image/jpeg";
#endif
    return result;
}

}
}
}
