/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_VIEW_H
#define EGT_VIEW_H

/**
 * @file
 * @brief View definition.
 */

#include <egt/frame.h>
#include <egt/slider.h>

namespace egt
{
inline namespace v1
{

/**
 * A scrollable view.
 *
 * A ScrolledView is a sort of kitchen window. You can have a large surface
 * area on the other side of the window, but you only see a small portion of
 * it through the window.  The surface can be scrolled, or panned, in a single
 * orientation to see the rest.
 *
 * This is used internally by Widgets, but can also be used directly.
 */
class ScrolledView : public Frame
{
public:

    explicit ScrolledView(const Rect& rect = Rect(),
                          orientation orient = orientation::horizontal);

    explicit ScrolledView(orientation orient);

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void resize(const Size& size) override;

    virtual Rect child_area() const override;

    /**
     * Get the current offset.
     *
     * @note The offset moves in the negative direction from zero.
     */
    int offset() const { return m_offset; }

    /**
     * Set the position.
     */
    virtual void set_offset(int offset);

    virtual bool scrollable() const;

    virtual ~ScrolledView();

protected:

    /**
     * Return the super rectangle that includes all of the child widgets.
     */
    Rect super_rect() const;

    /**
     * Resize the slider whenever the size of this changes.
     */
    void resize_slider();

    /**
     * Current offset of the view.
     */
    int m_offset{0};

    /**
     * Slider shown when scrollable.
     */
    Slider m_slider;

    /**
     * The orientation of the scroll.
     */
    orientation m_orient{orientation::horizontal};

    int m_start_offset{0};
};

}
}

#endif
