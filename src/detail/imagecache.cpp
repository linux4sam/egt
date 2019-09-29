/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/canvas.h"
#include "egt/detail/filesystem.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/math.h"
#include "egt/detail/resource.h"
#include "egt/utils.h"
#include "images/bmp/cairo_bmp.h"
#include <cassert>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

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
#include "egt/detail/svg.h"
#endif

#ifdef HAVE_SIMD
#include "Simd/SimdLib.hpp"
#endif

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

static std::vector<std::string> image_paths;

void add_search_path(const std::string& path)
{
    if (path.empty())
        return;

    auto newpath = path;

    if (*newpath.rbegin() != '/')
        newpath += '/';

    if (find(image_paths.begin(), image_paths.end(), newpath) == image_paths.end())
        image_paths.push_back(newpath);
}

std::string resolve_file_path(const std::string& filename)
{
    if (filename.empty())
        return filename;

    // we don't resolve absolute paths
    if (filename[0] == '/')
        return filename;

    for (auto& path : image_paths)
    {
        auto test = path + filename;

        SPDLOG_TRACE("looking at file {}", test);

        struct stat buf {};
        if (!stat(test.c_str(), &buf))
        {
            SPDLOG_TRACE("found file {}", test);
            return test;
        }
    }

    return filename;
}

shared_cairo_surface_t ImageCache::get(const std::string& filename,
                                       float hscale, float vscale, bool approximate)
{
    if (approximate)
    {
        hscale = ImageCache::round(hscale, 0.01);
        vscale = ImageCache::round(vscale, 0.01);
    }

    const string nameid = id(filename, hscale, vscale);

    auto i = m_cache.find(nameid);
    if (i != m_cache.end())
        return i->second;

    SPDLOG_DEBUG("image cache miss {} hscale:{} vscale:{}", filename, hscale, vscale);

    shared_cairo_surface_t image;

    if (detail::float_compare(hscale, 1.0f) &&
        detail::float_compare(vscale, 1.0f))
    {
        static_assert(CAIRO_HAS_PNG_FUNCTIONS == 1, "PNG support in cairo assumed.");

        if (filename.compare(0, 1, ":") == 0)
        {
            string name = filename;
            name.erase(0, 1);

            auto data = read_resource_data(name.c_str());
            if (!data)
                throw std::runtime_error("resource not found: " + name);

            auto mimetype = get_mime_type(data,
                                          read_resource_length(name.c_str()));
            SPDLOG_DEBUG("mimetype of {} is {}", name, mimetype);

            if (mimetype == "image/png")
            {
                image = shared_cairo_surface_t(
                            cairo_image_surface_create_from_png_stream(
                                read_resource_stream, const_cast<char*>(name.c_str())),
                            cairo_surface_destroy);
            }
#ifdef HAVE_LIBJPEG
            else if (mimetype == "image/jpeg")
            {
                image = shared_cairo_surface_t(
                            cairo_image_surface_create_from_jpeg_stream(
                                read_resource_stream, const_cast<char*>(name.c_str())),
                            cairo_surface_destroy);
            }
#endif
            else if (mimetype == "image/x-ms-bmp")
            {
                image = shared_cairo_surface_t(
                            cairo_image_surface_create_from_bmp_stream(
                                read_resource_stream, const_cast<char*>(name.c_str())),
                            cairo_surface_destroy);
            }
            else
            {
                throw std::runtime_error("unsupported mimetype for: " + name);
            }
        }
        else if (filename.compare(0, 1, "@") == 0)
        {
            string name = filename;
            name.erase(0, 1);

            static std::string egt_icons_dir = "32px";
            static std::once_flag env_flag;
            std::call_once(env_flag, []()
            {
                auto icons_dir = std::getenv("EGT_ICONS_DIRECTORY");
                if (icons_dir)
                {
                    auto dir = std::string(icons_dir);
                    if (!dir.empty())
                        egt_icons_dir = dir;
                }
            });

            if (name.find("/") != string::npos)
                name = resolve_file_path(name);
            else
                name = resolve_file_path(egt_icons_dir + "/" + name);

            if (!detail::exists(name))
                throw std::runtime_error("file not found: " + name);

            auto mimetype = get_mime_type(name);
            SPDLOG_DEBUG("mimetype of {} is {}", name, mimetype);

            if (mimetype == "image/png")
            {
                image = shared_cairo_surface_t(
                            cairo_image_surface_create_from_png(name.c_str()),
                            cairo_surface_destroy);
            }
#ifdef HAVE_LIBJPEG
            else if (mimetype == "image/jpeg")
            {
                image = shared_cairo_surface_t(
                            cairo_image_surface_create_from_jpeg(name.c_str()),
                            cairo_surface_destroy);
            }
#endif
            else if (mimetype == "image/x-ms-bmp")
            {
                image = shared_cairo_surface_t(
                            cairo_image_surface_create_from_bmp(name.c_str()),
                            cairo_surface_destroy);
            }
#ifdef HAVE_LIBRSVG
            else if (mimetype == "image/svg+xml")
            {
                image = load_svg(name);
            }
#endif
            else
            {
                throw std::runtime_error("unsupported mimetype for: " + name);
            }
        }
        else
        {
            string name = resolve_file_path(filename);

            if (!detail::exists(name))
                throw std::runtime_error("file not found: " + name);

            auto mimetype = get_mime_type(name);
            SPDLOG_DEBUG("mimetype of {} is {}", name, mimetype);

            if (mimetype == "image/png")
            {
                image = shared_cairo_surface_t(
                            cairo_image_surface_create_from_png(name.c_str()),
                            cairo_surface_destroy);
            }
#ifdef HAVE_LIBJPEG
            else if (mimetype == "image/jpeg")
            {
                image = shared_cairo_surface_t(
                            cairo_image_surface_create_from_jpeg(name.c_str()),
                            cairo_surface_destroy);
            }
#endif
            else if (mimetype == "image/x-ms-bmp")
            {
                image = shared_cairo_surface_t(
                            cairo_image_surface_create_from_bmp(name.c_str()),
                            cairo_surface_destroy);
            }
#ifdef HAVE_LIBRSVG
            else if (mimetype == "image/svg+xml")
            {
                image = load_svg(name);
            }
#endif
            else
            {
                throw std::runtime_error("unsupported mimetype for: " + name);
            }
        }
    }
    else
    {
        shared_cairo_surface_t back = get(filename, 1.0);

        double width = cairo_image_surface_get_width(back.get());
        double height = cairo_image_surface_get_height(back.get());

        experimental::code_timer(false, "scale: ", [&]()
        {
            image = scale_surface(back,
                                  width, height,
                                  width * hscale,
                                  height * vscale);
        });
    }

    if (cairo_surface_status(image.get()) != CAIRO_STATUS_SUCCESS)
    {
        stringstream ss;
        ss << cairo_status_to_string(cairo_surface_status(image.get()))
           << ": " << filename;
        throw std::runtime_error(ss.str());
    }

    m_cache.insert(std::make_pair(nameid, image));

    return image;
}

void ImageCache::clear()
{
    m_cache.clear();
}

float ImageCache::round(float v, float fraction)
{
    return floor(v) + floor((v - floor(v)) / fraction) * fraction;
}

string ImageCache::id(const string& filename, float hscale, float vscale)
{
    ostringstream ss;
    ss << filename << "-" << hscale * 100. << "-" << vscale * 100.;

    return ss.str();
}

#ifdef HAVE_SIMD
shared_cairo_surface_t
ImageCache::scale_surface(shared_cairo_surface_t old_surface,
                          float old_width, float old_height,
                          float new_width, float new_height)
{
    cairo_surface_flush(old_surface.get());

    auto new_surface = shared_cairo_surface_t(
                           cairo_surface_create_similar(old_surface.get(),
                                   CAIRO_CONTENT_COLOR_ALPHA,
                                   new_width,
                                   new_height),
                           cairo_surface_destroy);

    auto src = cairo_image_surface_get_data(old_surface.get());
    auto dst = cairo_image_surface_get_data(new_surface.get());

    SimdResizeBilinear(src,
                       old_width, old_height,
                       cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, old_width),
                       dst, new_width, new_height,
                       cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, new_width),
                       4);

    cairo_surface_mark_dirty(new_surface.get());

    return new_surface;
}
#else
shared_cairo_surface_t
ImageCache::scale_surface(shared_cairo_surface_t old_surface,
                          float old_width, float old_height,
                          float new_width, float new_height)
{
    auto new_surface = shared_cairo_surface_t(
                           cairo_surface_create_similar(old_surface.get(),
                                   CAIRO_CONTENT_COLOR_ALPHA,
                                   new_width,
                                   new_height),
                           cairo_surface_destroy);
    auto cr = shared_cairo_t(cairo_create(new_surface.get()),
                             cairo_destroy);

    /* Scale *before* setting the source surface (1) */
    cairo_scale(cr.get(),
                new_width / old_width,
                new_height / old_height);
    cairo_set_source_surface(cr.get(), old_surface.get(), 0, 0);

    /* To avoid getting the edge pixels blended with 0 alpha, which would
     * occur with the default EXTEND_NONE. Use EXTEND_PAD for 1.2 or newer (2)
     */
    cairo_pattern_set_extend(cairo_get_source(cr.get()), CAIRO_EXTEND_REFLECT);

    /* Replace the destination with the source instead of overlaying */
    cairo_set_operator(cr.get(), CAIRO_OPERATOR_SOURCE);

    /* Do the actual drawing */
    cairo_paint(cr.get());

    return new_surface;
}
#endif

std::string ImageCache::get_mime_type(const void* buffer, size_t length)
{
    string result;
#ifdef HAVE_LIBMAGIC
    magic_t magic = magic_open(MAGIC_MIME_TYPE);

    if (magic)
    {
        if (!magic_load(magic, nullptr))
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

std::string ImageCache::get_mime_type(const std::string& filename)
{
    string result;
#ifdef HAVE_LIBMAGIC
    magic_t magic = magic_open(MAGIC_MIME_TYPE);

    if (magic)
    {
        if (!magic_load(magic, nullptr))
        {
            auto mime = magic_file(magic, filename.c_str());
            if (mime)
                result = mime;
        }

        magic_close(magic);
    }
#else
    if (filename.find("png") != std::string::npos)
        result = "image/png";
    else if (filename.find("jpg") != std::string::npos)
        result = "image/jpeg";
#endif
    return result;
}

ImageCache& image_cache()
{
    static ImageCache cache;
    return cache;
}

}
}
}
