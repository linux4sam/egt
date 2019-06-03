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
 * Image resource used for drawing or displaying.
 *
 * This class by default shares the internal surface pointer with anything
 * else using the surface.  To force this class to keep its own copy, call
 * the copy() function.
 *
 * If a filename is used to construct an Image, the detail::ImageCache class
 * will be used.
 *
 * @ingroup media
 */
class Image
{
public:

    /**
     * @param respath Resource path.
     * @param hscale Horizontal scale of the image [0.0 - 1.0].
     * @param vscale Vertical scale of the image [0.0 - 1.0].
     */
    // cppcheck-suppress noExplicitConstructor
    Image(const std::string& respath = std::string(),
          double hscale = 1.0, double vscale = 1.0);

    /**
     * @param surface A pre-existing surface.
     *
     * This take a reference to the passed in image.
     *
     * @todo This is a broken API.  It's different from the constructor below.
     * Need consistency.
     */
    // cppcheck-suppress noExplicitConstructor
    Image(shared_cairo_surface_t surface);

    /**
     * @param surface A pre-existing surface.
     *
     * This will not own the passed in pointer or surface, and instead will make
     * a copy of the surface.
     */
    // cppcheck-suppress noExplicitConstructor
    Image(cairo_surface_t* surface);

    Image(const Image&) = default;
    Image(Image&&) = default;
    Image& operator=(const Image&) = default;
    Image& operator=(Image&&) = default;

    /**
     * Scale the image.
     *
     * Change the scale of the image, similar to calling Image::resize().
     *
     * This scales relative to the original size of the image.  Not the result
     * of any subsequent Image::resize() or Image::scale() call.
     *
     * @param hscale Horizontal scale of the image [0.0 - 1.0].
     * @param vscale Vertical scale of the image [0.0 - 1.0].
     * @param approximate Approximate the scale to increase image cache
     *            hit efficiency.
     */
    virtual void scale(double hscale, double vscale,
                       bool approximate = false);

    /**
     * Resize the image to the specified absolute size.
     *
     * This scales relative to the original size of the image.  Not the result
     * of any subsequent Image::resize() or Image::scale() call.
     *
     * @param size The new size of the image.
     */
    virtual void resize(const Size& size)
    {
        if (this->size() != size)
        {
            double hs = static_cast<double>(size.w) / static_cast<double>(m_orig_size.w);
            double vs = static_cast<double>(size.h) / static_cast<double>(m_orig_size.h);
            scale(hs, vs);
        }
    }

    /**
     * Get the horizontal scale value.
     */
    inline double hscale() const { return m_hscale; }

    /**
     * Get the vertical scale value.
     */
    inline double vscale() const { return m_vscale; }

    /**
     * Get the absolute size of the image.
     */
    inline Size size() const
    {
        if (empty())
            return {};

        return Size(cairo_image_surface_get_width(surface().get()),
                    cairo_image_surface_get_height(surface().get()));
    }

    /**
     * Returns true if no internal surface is set.
     */
    inline bool empty() const
    {
        return !surface();
    }

    /**
     * Get a reference to the internal image surface.
     */
    inline shared_cairo_surface_t surface() const
    {
        if (m_surface_local.get())
            return m_surface_local;
        return m_surface;
    }

    /**
     * Get the original size of the image before any Image::resize() or
     * Image::scale() calls.
     */
    inline Size size_orig() const { return m_orig_size; }

    /**
     * This function must be called any time the surface is going to be
     * modified.  It's safe to call this function when not necessary, and in
     * any event it will ensure this Image contains a unique copy of the
     * surface.
     *
     * @todo Tricky API.
     */
    virtual void copy();

    virtual ~Image() = default;

protected:

    /**
     * If a respath was used, the respath.
     */
    std::string m_respath;

    /**
     * Horizontal scale value, compared to original.
     */
    double m_hscale{1.0};

    /**
     * Vertical scale value, compared to original.
     */
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
     * Original image size.
     */
    Size m_orig_size;
};

}
}

#endif
