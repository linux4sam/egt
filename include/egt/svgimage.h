/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SVGIMAGE_H
#define EGT_SVGIMAGE_H

/**
 * @file
 * @brief Working with images.
 */

#include <cairo.h>
#include <egt/geometry.h>
#include <egt/image.h>
#include <egt/types.h>
#include <string>
#include <memory>

namespace egt
{
inline namespace v1
{

/**
 * An SVG image.
 *
 * This class is not just for working with a single Image. It allows loading an
 * SVG file or data once, and then rendering any number of images at different
 * sizes or made up of different objects from the SVG file.
 */
class SvgImage
{
public:

    SvgImage();

    /**
     * @param[in] respath Resource path.
     * @param[in] size Size of the image.
     *
     * @see SvgImage::set_size() for more information about specifying size.
     */
    // cppcheck-suppress noExplicitConstructor
    SvgImage(const std::string& respath, const SizeF& size = {});

    operator Image() const;

    /**
     * Render a specific element in the SVG file.
     *
     * @param[in] id The id of the SVG element.
     * @param[in] rect Optional rect to clip to.
     */
    virtual Image id(const std::string& id, const RectF& rect = {}) const;

    /**
     * Get the position and size of an element in the SVG.
     *
     * @param[in] id The id of the SVG element.
     */
    virtual RectF id_box(const std::string& id) const;

    /**
     * Test if an element exists in the SVG image.
     *
     * @param[in] id The id of the SVG element.
     */
    virtual bool id_exists(const std::string& id) const;

    /**
     * Get the SVG size.
     */
    SizeF size() const;

    /**
     * Set the size you want any rendered Image to be.
     *
     * This is somewhat special.  If you specify zero for one of the dimensions,
     * the other dimension will be automatically computed keeping the same
     * aspect ratio.
     *
     * If both dimensions are zero, the default size will be loaded from the SVG
     * file.
     */
    inline void set_size(const SizeF& size)
    {
        m_size = size;
    }

    /**
     * Set the respath.
     *
     * If this results in changing the respath, this will cause a re-load of the
     * SVG file
     */
    void set_respath(const std::string& respath);

    virtual ~SvgImage();

protected:

    /**
     * Load the SVG file.
     */
    void load();

    /**
     * Render and return a new surface.
     */
    shared_cairo_surface_t render(const std::string& id = {}, const RectF& rect = {}) const;

    struct SvgImpl;

    /**
     * Implementation pointer.
     */
    std::unique_ptr<SvgImpl> m_impl;

    /**
     * User set size.
     * Use size() to get the calculated size.
     */
    SizeF m_size;

    /**
     * User set respath.
     */
    std::string m_respath;
};

}
}

#endif
