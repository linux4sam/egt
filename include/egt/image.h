/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_IMAGE_H
#define EGT_IMAGE_H

/**
 * @file
 * @brief Working with images.
 */

#include <cairo.h>
#include <egt/geometry.h>
#include <egt/painter.h>
#include <string>

namespace egt
{
inline namespace v1
{
/**
 * Image used for drawing or displaying.
 *
 * This class by default shares the internal surface pointer with anything
 * else using the surface.  To force this class to keep its own copy, call
 * the copy() function.
 *
 * If a filename is used to contruct an Image, the detail::ImageCache class
 * will be used.
 */
class Image
{
public:
    explicit Image(const std::string& filename = std::string(),
                   double hscale = 1.0, double vscale = 1.0);

    explicit Image(shared_cairo_surface_t surface);

    explicit Image(cairo_surface_t* surface);

    /**
     * Scale the image.
     *
     * Change the size of the widget, similar to calling resize().
     *
     * @param[in] hscale Horizontal scale, with 1.0 being 100%.
     * @param[in] vscale Vertical scale, with 1.0 being 100%.
     * @param[in] approximate Approximate the scale to increase image cache
     *            hit efficiency.
     */
    virtual void scale(double hscale, double vscale,
                       bool approximate = false);

    virtual void resize(const Size& size)
    {
        if (this->size() != size)
        {
            double hs = (double)size.w / (double)m_orig_size.w;
            double vs = (double)size.h / (double)m_orig_size.h;
            scale(hs, vs);
        }
    }

    /**
     * Get the horizontal scale value.
     */
    double hscale() const { return m_hscale; }

    Size size() const
    {
        if (empty())
            return Size();

        return Size(cairo_image_surface_get_width(surface().get()),
                    cairo_image_surface_get_height(surface().get()));
    }

    /**
     * Get the vertical scale value.
     */
    double vscale() const { return m_vscale; }

    /**
     * Returns true if no internal surface is set.
     */
    bool empty() const
    {
        return !surface();
    }

    shared_cairo_surface_t surface() const
    {
        if (m_surface_local.get())
            return m_surface_local;
        return m_surface;
    }

    Size size_orig() const { return m_orig_size; }

    /**
     * This function must be called any time the surface is going to be
     * modified.  It's safe to call this function when not necessary, and in
     * any event it will ensure this Image contains a unique copy of the
     * surface.
     */
    virtual void copy();

    virtual ~Image();

protected:

    /**
     * If a filename was used, the filename.
     */
    std::string m_filename;

    double m_hscale{1.0};
    double m_vscale{1.0};

    /**
     * Shared surface pointer.
     */
    shared_cairo_surface_t m_surface;

    /**
     * Local surface pointer.
     */
    shared_cairo_surface_t m_surface_local;

    /**
     * Original image size, used for scaling through a resize() call.
     */
    Size m_orig_size;
};

}
}

#endif
