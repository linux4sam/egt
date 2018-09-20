/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "imagecache.h"
#include "resource.h"
#include "utils.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace mui
{
    shared_cairo_surface_t ImageCache::get(const std::string& filename,
					   float hscale, float vscale, bool approximate)
    {
	if (approximate)
	{
	    hscale = ImageCache::round(hscale, 0.01);
	    vscale = ImageCache::round(vscale, 0.01);
	}

	string name = id(filename, hscale, vscale);

	auto i = m_cache.find(name);
	if (i != m_cache.end())
	    return i->second;

	DBG("image cache miss: " << filename << " hscale:" << hscale << " vscale:" << vscale);

	shared_cairo_surface_t image;

	if (hscale == 1.0 && vscale == 1.0)
	{
	    std::string::size_type i = filename.find(":");
	    if (i == 0)
	    {
		string name = filename;
		name.erase(i, 1);
		image = shared_cairo_surface_t(
		    cairo_image_surface_create_from_png_stream(
			read_resource_stream,(void*)name.c_str()),
		    cairo_surface_destroy);
	    }
	    else
	    {
		image = shared_cairo_surface_t(
		    cairo_image_surface_create_from_png(filename.c_str()),
		    cairo_surface_destroy);
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

	m_cache.insert(std::make_pair(name, image));

	return image;
    }

    void ImageCache::clear()
    {
	m_cache.clear();
    }

    float ImageCache::round(float v, float fraction)
    {
	return floor(v) + floor( (v-floor(v))/fraction) * fraction;
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
