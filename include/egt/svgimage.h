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

#include <egt/detail/meta.h>
#include <egt/geometry.h>
#include <egt/image.h>
#include <egt/types.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

/**
 * An SVG image.
 *
 * This class is not just for working with a single Image. It allows loading an
 * SVG file or data once, and then rendering any number of images at different
 * sizes or made up of different elements from the SVG file.
 *
 * @code{.cpp}
 * SvgImage svg("file.svg");
 * @endcode
 *
 * @note Some standard effects in SVG files are expensive to render. For
 * example, some patterns and gradients.  In those cases, you can convert
 * elements in the SVG itself to raster images to speed up rendering here.
 */
class EGT_API SvgImage
{
public:

    SvgImage();

    /**
     * @param uri Resource path. @see @ref resources
     * @param[in] size Size of the image.
     *
     * @see SvgImage::size() for more information about specifying size.
     */
    // cppcheck-suppress noExplicitConstructor
    SvgImage(const std::string& uri, const SizeF& size = {});

    /**
     * Overload to convert to an Image.
     *
     * This is the same as calling render().
     */
    operator Image() const;

    /**
     * Render the SVG optionally with only the specified element id in the SVG
     * file.
     *
     * @note If no rect is specified or an empty rect is specified, the raster
     * Image returned will be the same size as the entire SVG.
     *
     * @param[in] id Optional id of the SVG element to render.  If no id is
     *               specified, all elements in the SVG will be rendered.
     * @param[in] rect Optional rect to clip to.
     */
    virtual Image render(const std::string& id = {}, const RectF& rect = {}) const;

    /**
     * Render the image of the specific element in the SVG file.
     *
     * This is the same as calling:
     * @code{.cpp}
     * render("id", svg.id_box("id"))
     * @endcode
     *
     * @note The size of the returned image will be the size of the specific
     * element in the SVG file.
     *
     * @see render().
     */
    virtual Image id_image(const std::string& id) const
    {
        return this->render(id, id_box(id));
    }

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
     * @return true if found
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
    inline void size(const SizeF& size)
    {
        m_size = size;
    }

    /**
     * Set the URI.
     *
     * If this results in changing the URI for this instance, this will cause a
     * re-load of the SVG file
     */
    void uri(const std::string& uri);

    virtual ~SvgImage();

protected:

    /**
     * Load the SVG file.
     */
    void load();

    /**
     * Render and return a new surface.
     */
    shared_cairo_surface_t do_render(const std::string& id = {}, const RectF& rect = {}) const;

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
     * User set URI.
     */
    std::string m_uri;
};

}
}

#endif
