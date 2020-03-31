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
 * Widget that shows or animates sprite, or frame, strips from a single sprite
 * sheet.
 *
 * A sprite sheet is an image that consists of several smaller images called
 * sprites, or frames. Combining the small images in one big image improves
 * performance, reduces the memory usage, and speeds up initialization time.
 * EGT also has the capability to directly take advantage of hardware
 * acceleration to quickly switch between different sprites, or frames, without
 * having to do any redrawing or copying in software.
 *
 * This class supports dividing up a single sprite sheet into different strips,
 * or animations.  Each strip consisting of one or more frames.  For example,
 * imagine you have several different animations of a character - one running
 * left, one jumping, one standing, etc. You can put all of these sequences of
 * frames, or strips, into a single sprite sheet image and then use this class
 * to configure the different animations, switch between them, and animate them
 * easily.
 *
 * A requirement that this class imposes on sprite sheets is that all frames
 * must be the same size among all configured strips.  If you have different
 * frame sizes, you can still use the same sprite sheet image, but you must use
 * different instances of this class.  Frame sequences, or strips, are expected
 * to be from left to right and wrap to the next "line".
 *
 * There is no requirement on how many frames are on a line or how many rows of
 * frames there are.
 */
class EGT_API Sprite : public Window
{
public:

    /**
     * @param[in] hint Requested Window type. This only applies if this Window
     *            will be responsible for creating a backing screen.  This is
     *            only a hint.
     */
    explicit Sprite(WindowHint hint = WindowHint::automatic);

    /**
     * @param[in] image The image used to read frames from.
     * @param[in] frame_size The size of each frame.
     * @param[in] frame_count The number of frames available in the strip.
     * @param[in] frame_point If the first frame of this strip is not in the top
     *            left corner of the image, specify where the top left corner is.
     * @param[in] hint Requested Window type. This only applies if this Window
     *            will be responsible for creating a backing screen.  This is
     *            only a hint.
     */
    Sprite(const Image& image, const Size& frame_size,
           int frame_count, const Point& frame_point = {},
           WindowHint hint = WindowHint::automatic);

    EGT_OPS_NOCOPY_MOVE(Sprite);
    ~Sprite() noexcept override;

    void draw(Painter& painter, const Rect& rect) override;

    void paint(Painter& painter) override;

    /**
     * Initialize the sprite with new configuration.
     *
     * @param[in] image The image used to read frames from.
     * @param[in] frame_size The size of each frame.
     * @param[in] frame_count The number of frames available in the strip.
     * @param[in] frame_point If the first frame of this strip is not in the top
     *            left corner of the image, specify where the top left corner is.
     */
    void init(const Image& image, const Size& frame_size,
              int frame_count, const Point& frame_point = {});

    /**
     * Show the frame specified by the index.
     */
    void show_frame(int index);

    /**
     * Get a surface for the current frame.
     *
     * @warning This does not return the whole image surface.  Just the current
     * frame.
     */
    shared_cairo_surface_t surface() const;

    /**
     * Advance to the next frame in the strip.
     */
    void advance();

    /**
     * Returns true if the current frame is the last frame.
     */
    bool is_last_frame() const;

    /**
     * Returns the number of frames in the current strip.
     */
    uint32_t frame_count() const;

    /**
     * Change the strip to the specified id.
     */
    void change_strip(uint32_t id);

    /**
     * Add a new strip.
     *
     * @param[in] frame_count The number of frames available in the strip.
     * @param[in] frame_point If the first frame of this strip is not in the top
     * left corner of the image, specify where the top left corner is.
     */
    uint32_t add_strip(int frame_count, const Point& frame_point = {});

protected:

    /// @private
    void allocate_screen() override
    {
        Window::allocate_screen();
    }

    /// Create the internal implementation.
    void create_impl(const Image& image, const Size& frame_size,
                     int frame_count, const Point& frame_point);

    /// @private
    std::unique_ptr<detail::SpriteImpl> m_simpl;

    friend class detail::SoftwareSprite;
    friend class detail::HardwareSprite;
};

}
}

#endif
