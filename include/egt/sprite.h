/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SPRITE_H
#define EGT_SPRITE_H

/**
 * @file
 * @brief Working with sprites.
 */

#include <egt/detail/meta.h>
#include <egt/window.h>
#include <memory>

namespace egt
{
inline namespace v1
{
class Painter;
class Image;

namespace detail
{
class SpriteImpl;
class SoftwareSprite;
class HardwareSprite;
}

/**
 * Sprite widget that animates frames from an image using one or more configured
 * strips.
 */
class EGT_API Sprite : public Window
{
public:

    Sprite();

    /**
     * @param[in] image The image used to read frames from.
     * @param[in] frame_size The size of each frame.
     * @param[in] frame_count The number of frames available in the strip.
     * @param[in] frame_point If the first frame of this strip is not in the top
     * left corner of the image, specify where the top left corner is.
     */
    Sprite(const Image& image, const Size& frame_size,
           int frame_count, const Point& frame_point = {});

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void paint(Painter& painter) override;

    /**
     * @param[in] image The image used to read frames from.
     * @param[in] frame_size The size of each frame.
     * @param[in] frame_count The number of frames available in the strip.
     * @param[in] frame_point If the first frame of this strip is not in the top
     * left corner of the image, specify where the top left corner is.
     */
    virtual void init(const Image& image, const Size& frame_size,
                      int frame_count, const Point& frame_point = {});

    /**
     * Show the frame specified by the index.
     */
    virtual void show_frame(int index);

    /**
     * Get a surface for the current frame.
     *
     * @warning This does not return the whole image surface.  Just the current
     * frame.
     */
    virtual shared_cairo_surface_t surface() const;

    /**
     * Advance to the next frame in the strip.
     */
    virtual void advance();

    /**
     * Returns true if the current frame is the last frame.
     */
    virtual bool is_last_frame() const;

    /**
     * Returns the number of frames in the current strip.
     */
    virtual uint32_t frame_count() const;

    /**
     * Change the strip to the specified id.
     */
    virtual void set_strip(uint32_t id);

    /**
     * Add a new strip.
     *
     * @param[in] frame_count The number of frames available in the strip.
     * @param[in] frame_point If the first frame of this strip is not in the top
     * left corner of the image, specify where the top left corner is.
     */
    virtual uint32_t add_strip(int frame_count, const Point& frame_point = {});

    virtual ~Sprite();

protected:

    virtual void allocate_screen() override
    {
        Window::allocate_screen();
    }

    void create_impl(const Image& image, const Size& frame_size,
                     int frame_count, const Point& frame_point);

    std::unique_ptr<detail::SpriteImpl> m_simpl;

    friend class detail::SoftwareSprite;
    friend class detail::HardwareSprite;
};

}
}

#endif
