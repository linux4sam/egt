/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/imagecache.h"
#include "egt/resource.h"
#include "egt/utils.h"
#include <cassert>
#include <iostream>
#include <sstream>

#ifdef HAVE_LIBJPEG
#ifdef __cplusplus
extern "C" {
#endif
#include "cairo_jpg.h"
#ifdef __cplusplus
}
#endif
#endif

using namespace std;

namespace egt
{

    auto ICON_PATH = "../share/egt/icons/";

    static string image_path = "";
    void set_image_path(const std::string& path)
    {
        image_path = path;
    }

    namespace detail
    {

        shared_cairo_surface_t ImageCache::get(const std::string& filename,
                                               float hscale, float vscale, bool approximate)
        {
            if (approximate)
            {
                hscale = ImageCache::round(hscale, 0.01);
                vscale = ImageCache::round(vscale, 0.01);
            }

            const string name = id(filename, hscale, vscale);

            auto i = m_cache.find(name);
            if (i != m_cache.end())
                return i->second;

            DBG("image cache miss: " << filename << " hscale:" << hscale << " vscale:" << vscale);

            shared_cairo_surface_t image;

            if (hscale == 1.0 && vscale == 1.0)
            {
                assert(CAIRO_HAS_PNG_FUNCTIONS == 1);

                if (filename.compare(0, 1, ":") == 0)
                {
                    string name = filename;
                    name.erase(0, 1);

                    /**
                     * @todo This should use MIME info and not filename.
                     */
                    if (name.find("png") != std::string::npos)
                    {
                        image = shared_cairo_surface_t(
                                    cairo_image_surface_create_from_png_stream(
                                        read_resource_stream, (void*)name.c_str()),
                                    cairo_surface_destroy);
                    }
#ifdef HAVE_LIBJPEG
                    else if (name.find("jpg") != std::string::npos)
                    {
                        image = shared_cairo_surface_t(
                                    cairo_image_surface_create_from_jpeg_stream(
                                        read_resource_stream, (void*)name.c_str()),
                                    cairo_surface_destroy);
                    }
#endif
                    else
                    {
                        ERR("could not load file " << filename);
                        assert(!"unsupported file type");
                    }
                }
                else if (filename.compare(0, 1, "@") == 0)
                {
                    string name = filename;
                    name.erase(0, 1);
                    name = ICON_PATH + name;

                    /**
                     * @todo This should use MIME info and not filename.
                     */
                    if (name.find("png") != std::string::npos)
                    {
                        image = shared_cairo_surface_t(
                                    cairo_image_surface_create_from_png(name.c_str()),
                                    cairo_surface_destroy);
                    }
#ifdef HAVE_LIBJPEG
                    else if (name.find("jpg") != std::string::npos)
                    {
                        image = shared_cairo_surface_t(
                                    cairo_image_surface_create_from_jpeg(name.c_str()),
                                    cairo_surface_destroy);
                    }
#endif
                    else
                    {
                        ERR("could not load file " << filename);
                        assert(!"unsupported file type");
                    }
                }
                else
                {
                    string name;
                    if (filename[0] == '/' || filename[0] == '.')
                        name = filename;
                    else
                        name = image_path + filename;
                    DBG("loading: " << name);

                    if (name.find("png") != std::string::npos)
                    {
                        image = shared_cairo_surface_t(
                                    cairo_image_surface_create_from_png(name.c_str()),
                                    cairo_surface_destroy);
                    }
#ifdef HAVE_LIBJPEG
                    else if (name.find("jpg") != std::string::npos)
                    {
                        image = shared_cairo_surface_t(
                                    cairo_image_surface_create_from_jpeg(name.c_str()),
                                    cairo_surface_destroy);
                    }
#endif
                    else
                    {
                        ERR("could not load file " << filename);
                        assert(!"unsupported file type");
                    }
                }
            }
            else
            {
                shared_cairo_surface_t back = get(filename, 1.0);

                double width = cairo_image_surface_get_width(back.get());
                double height = cairo_image_surface_get_height(back.get());

                image = scale_surface(back,
                                      width, height,
                                      width * hscale,
                                      height * vscale);
            }

            if (cairo_surface_status(image.get()) != CAIRO_STATUS_SUCCESS)
            {
                stringstream ss;
                ss << cairo_status_to_string(cairo_surface_status(image.get()))
                   << ": " << filename;
                throw std::runtime_error(ss.str());
            }

            m_cache.insert(std::make_pair(name, image));

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

        shared_cairo_surface_t
        ImageCache::scale_surface(shared_cairo_surface_t old_surface,
                                  int old_width, int old_height,
                                  int new_width, int new_height)
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
                        (double)new_width / old_width,
                        (double)new_height / old_height);
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


        ImageCache image_cache;

    }
}
