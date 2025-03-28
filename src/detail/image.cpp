/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/egtlog.h"
#include "detail/eraw.h"
#include "egt/app.h"
#include "egt/detail/filesystem.h"
#include "egt/detail/image.h"
#include "egt/resource.h"
#include "images/bmp/cairo_bmp.h"
#include <fstream>
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
static constexpr auto MIME_JPEG = "image/jpeg";
static constexpr auto MIME_PNG = "image/png";
static constexpr auto MIME_GIF = "image/gif";
#ifdef HAVE_LIBRSVG
static constexpr auto MIME_SVGXML = "image/svg+xml";
static constexpr auto MIME_SVG = "image/svg";
#endif
static constexpr auto MIME_PDF = "application/pdf";
static constexpr auto MIME_ZIP = "application/zip";
static constexpr auto MIME_GZIP = "application/gzip";
static constexpr auto MIME_ERAW = "image/eraw";

static Surface copy_cairo_surface(cairo_surface_t* surface)
{
    Size size(cairo_image_surface_get_width(surface),
              cairo_image_surface_get_height(surface));
    auto format = detail::egt_format(cairo_image_surface_get_format(surface));

    Surface image(size, format);
    memcpy(image.data(), cairo_image_surface_get_data(surface),
           size.height() * cairo_image_surface_get_stride(surface));

    return image;
}

EGT_API Surface load_image_from_memory(const unsigned char* data,
                                       size_t len,
                                       const std::string& name)
{
    if (!data || !len)
        return {};

    Surface image;

    const auto mimetype = get_mime_type(data, len);
    if (mimetype.empty())
        throw std::runtime_error("unable to determine mimetype for: " + name);

    EGTLOG_DEBUG("mimetype of {} is {}", name, mimetype);

    if (mimetype == MIME_BMP)
    {
        StreamObject stream = {data, len, 0};
        unique_cairo_surface_t surface(
            cairo_image_surface_create_from_bmp_stream(read_stream, &stream));
        image = copy_cairo_surface(surface.get());
    }
    else if (mimetype == MIME_ERAW)
    {
        image = load_eraw(data, len);
    }
#ifdef HAVE_LIBJPEG
    else if (mimetype == MIME_JPEG)
    {
        StreamObject stream = {data, len, 0};
        unique_cairo_surface_t surface(
            cairo_image_surface_create_from_jpeg_stream(read_stream, &stream));
        image = copy_cairo_surface(surface.get());
    }
#endif
#if CAIRO_HAS_PNG_FUNCTIONS == 1
    else if (mimetype == MIME_PNG)
    {
        StreamObject stream = {data, len, 0};
        unique_cairo_surface_t surface(
            cairo_image_surface_create_from_png_stream(read_stream, &stream));
        image = copy_cairo_surface(surface.get());
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

Surface load_image_from_resource(const std::string& name)
{
    if (!ResourceManager::instance().exists(name.c_str()))
        throw std::runtime_error("resource not found: " + name);

    ResourceManager::instance().stream_reset(name.c_str());

    return load_image_from_memory(ResourceManager::instance().data(name.c_str()),
                                  ResourceManager::instance().size(name.c_str()),
                                  name);
}

Surface load_image_from_filesystem(const std::string& path)
{
    if (!detail::exists(path))
        throw std::runtime_error("file not found: " + path);

    const auto mimetype = get_mime_type(path);
    if (mimetype.empty())
        throw std::runtime_error("unable to determine mimetype for: " + path);
    EGTLOG_DEBUG("mimetype of {} is {}", path, mimetype);

    Surface image;

    if (mimetype == MIME_BMP)
    {
        unique_cairo_surface_t surface(
            cairo_image_surface_create_from_bmp(path.c_str()));
        image = copy_cairo_surface(surface.get());
    }
    else if (mimetype == MIME_ERAW)
    {
        image = load_eraw(path);
    }
#ifdef HAVE_LIBJPEG
    else if (mimetype == MIME_JPEG)
    {
        unique_cairo_surface_t surface(
            cairo_image_surface_create_from_jpeg(path.c_str()));
        image = copy_cairo_surface(surface.get());
    }
#endif
#if CAIRO_HAS_PNG_FUNCTIONS == 1
    else if (mimetype == MIME_PNG)
    {
        unique_cairo_surface_t surface(
            cairo_image_surface_create_from_png(path.c_str()));
        image = copy_cairo_surface(surface.get());
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

EGT_API Surface load_image_from_network(const std::string& url)
{
    Surface image;

#ifdef HAVE_LIBCURL
    auto buffer = experimental::load_file_from_network<std::vector<unsigned char>>(url);

    if (!buffer.empty())
        image = load_image_from_memory(reinterpret_cast<const unsigned char*>(buffer.data()),
                                       buffer.size(),
                                       url);
#else
    detail::ignoreparam(url);
    detail::warn("network support not available");
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

/*
 * Basic and quick mime type detection of files or raw data.
 *
 * libmagic is wonderfully complete and accurate.  However, it can be a bit slow
 * due to completeness - sometimes reading entire database files before being
 * able to determine mime type.  This is a basic implementation that only
 * supports a couple mime types using basic magic signature detection.
 */
struct BasicMimeTypeDetector
{
    std::string get_mime_type(const void* buffer, size_t length)
    {
        if (length >= 4)
        {
            auto signature = reinterpret_cast<const uint32_t*>(buffer);
            auto mime = mime_type(ntoh(*signature));
            if (mime)
                return mime;
        }

        return {};
    }

    std::string get_mime_type(const std::string& path)
    {
        std::string result;
        std::ifstream in = std::ifstream(path, std::ios::binary);
        if (in)
        {
            char bytes[4] = {};
            in.read(bytes, 4);
            if (in.gcount() == 4)
                result = get_mime_type(bytes, 4);
        }

        return result;
    }

private:

    template<typename T>
    T ntoh(T value)
    {
#if BYTE_ORDER == LITTLE_ENDIAN
        if (sizeof(T) == 8)
            return __builtin_bswap64(value);
        if (sizeof(T) == 4)
            return __builtin_bswap32(value);
        if (sizeof(T) == 2)
            return __builtin_bswap16(value);
        if (sizeof(T) == 1)
            return value;
#else
        return value;
#endif
    }

    static const char* mime_type(uint32_t signature)
    {
        EGTLOG_DEBUG("signature: {}", signature);
        switch (signature)
        {
        case 0x89504E47:
            return MIME_PNG;
        case 0x47494638:
            // technically, to be complete there are 2 variants
            // 47 49 46 38 37 61
            // 47 49 46 38 39 61
            return MIME_GIF;
        case 0x25504446:
            return MIME_PDF;
#ifdef HAVE_LIBRSVG
        case 0x3C737667:
            return MIME_SVG;
#endif
        case 0xFFD8FFDB:
        case 0xFFD8FFE0:
            return MIME_JPEG;
        case 0x504B0304:
            return MIME_ZIP;
        case 0xA22A5050:
            return MIME_ERAW;
        default:
            break;
        }

        switch ((signature >> 16) & 0xffff)
        {
        case 0x1F8B:
            return MIME_GZIP;
        case 0x424D:
            return MIME_BMP;
        }

        return nullptr;
    }
};

std::string get_mime_type(const void* buffer, size_t length)
{
    if (!buffer || !length)
        return {};

    BasicMimeTypeDetector detector;
    std::string result = detector.get_mime_type(buffer, length);
    if (!result.empty())
        return result;

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
    BasicMimeTypeDetector detector;
    std::string result = detector.get_mime_type(path);
    if (!result.empty())
        return result;

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
#endif

    return result;
}

}
}
}
