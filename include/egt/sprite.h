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

#include <egt/detail/spriteimpl.h>
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
class SoftwareSprite;
class HardwareSprite;
}

/**
 * Sprite widget that animates using a spite sheet Image.
 */
class Sprite : public Window
{
public:
    Sprite(const Image& image, const Size& frame_size,
           int framecount, const Point& frame_point,
           const Point& point = Point());

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void show_frame(int index);

    virtual void paint(Painter& painter) override;

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
     */
    virtual uint32_t add_strip(int framecount, const Point& point);

    virtual ~Sprite()
    {}

protected:

    virtual void allocate_screen() override
    {
        Window::allocate_screen();
    }

    void create_impl(const Image& image, const Size& frame_size,
                     int framecount, const Point& frame_point);

    std::unique_ptr<detail::SpriteImpl> m_simpl;

    friend class detail::SpriteImpl;
    friend class detail::SoftwareSprite;
    friend class detail::HardwareSprite;

private:

    Sprite() = delete;
};

}
}

#endif
