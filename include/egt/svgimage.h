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
 * For using individual elements from a SVG file and render them separately
 * the document must be created with element IDs. In the dashboard example for
 * instance, one of the \p dash_background.svg element has the attribute:
 * @code{.svg}
 * id="right_blink"
 * @endcode
 * Several methods in the SvgImage class allow the use of such elements when
 * the \p id parameter is present. Element IDs must follow the syntax of an URL
 * fragment identifier. For example, use "#right_blink" (hash right_blink) to
 * obtain a handle on the SVG element mentioned just above; which gives:
 * @code{.cpp}
 * SvgImage custom1svg("file:dash_background.svg", SizeF(200, 0));
 * auto img_box = custom1svg.id_box("#right_blink");
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
    explicit SvgImage(const std::string& uri, const SizeF& size = {});

    SvgImage(const SvgImage&) = delete;
    SvgImage& operator=(const SvgImage&) = delete;
    SvgImage(SvgImage&&) noexcept;
    SvgImage& operator=(SvgImage&&) noexcept;

    ~SvgImage() noexcept;

    /**
     * Overload to convert to an Image.
     *
     * This is the same as calling render().
     */
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    operator Image() const;

    /**
     * Render the SVG optionally with only the specified element id in the SVG
     * file.
     *
     * @note If no rect is specified or an empty rect is specified, the raster
     * Image returned will be the same size as the entire SVG.
     *
     * @param[in] id Optional id of the SVG element to render.  If no id is
     *               specified, all elements in the SVG will be rendered. \n
     *               Element IDs must follow the syntax of an URL fragment
     *               identifier.  For example, use "#right_blink" (hash right_blink)
     *               to obtain a handle on the SVG element with
     *               id="right_blink".
     * @param[in] rect Optional rect to clip to.
     */
    EGT_NODISCARD Image render(const std::string& id = {}, const RectF& rect = {}) const;

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
    EGT_NODISCARD Image id_image(const std::string& id) const
    {
        return this->render(id, id_box(id));
    }

    /**
     * Get the position and size of an element in the SVG.
     *
     * @param[in] id The id of the SVG element. \n
     *               Element IDs must follow the syntax of an URL fragment
     *               identifier.  For example, use "#right_blink" (hash right_blink)
     *               to obtain a handle on the SVG element with
     *               id="right_blink".
     */
    EGT_NODISCARD RectF id_box(const std::string& id) const;

    /**
     * Test if an element exists in the SVG image.
     *
     * @param[in] id The id of the SVG element. \n
     *               Element IDs must follow the syntax of an URL fragment
     *               identifier.  For example, use "#right_blink" (hash right_blink)
     *               to obtain a handle on the SVG element with
     *               id="right_blink".
     * @return true if found
     */
    EGT_NODISCARD bool id_exists(const std::string& id) const;

    /**
     * Get the SVG size.
     */
    EGT_NODISCARD SizeF size() const;

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
    void size(const SizeF& size)
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

protected:

    /// Load the SVG file.
    void load();

    /**
     * Render and return a new surface.
     */
    EGT_NODISCARD shared_cairo_surface_t do_render(const std::string& id = {}, const RectF& rect = {}) const;

    struct SvgImpl;

    /// Implementation pointer.
    std::unique_ptr<SvgImpl> m_impl;

    /**
     * User set size.
     * Use size() to get the calculated size.
     */
    SizeF m_size;

    /// User set URI.
    std::string m_uri;
};

}
}

#endif
