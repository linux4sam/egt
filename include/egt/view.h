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

#include <egt/detail/mousegesture.h>
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
 * This is used internally by Widgets, but can also be used directly.  For
 * example, if ther are too many elements in a ListBox to be displayed due
 * to the size of the widget, the View usage in a ListBox makes the list
 * scrollable.
 */
class ScrolledView : public Frame
{
public:

    explicit ScrolledView(const Rect& rect = Rect());

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    virtual void resize(const Size& size) override;

    /**
     * Get the position.
     */
    int position() const { return m_offset; }

    /**
     * Set the position.
     */
    virtual void set_position(int offset);

    virtual ~ScrolledView();

protected:

    Rect super_rect() const;

    void resize_slider();

    /**
     * Current offset of the view.
     */
    int m_offset{0};

    /**
     * The orientation of the scroll.
     * @todo Only horizontal is currently implemented.
     */
    orientation m_orientation{orientation::HORIZONTAL};

    Slider m_slider;

    using Swipe = detail::MouseGesture<int>;

    Swipe m_mouse;
};

}
}

#endif
