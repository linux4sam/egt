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
 * @brief A scrollable view.
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

    /**
     * @param[in] rect Rectangle for the widget.
     * @param[in] orient Vertical or horizontal orientation.
     */
    explicit ScrolledView(const Rect& rect = Rect(),
                          orientation orient = orientation::horizontal);

    /**
     * @param[in] orient Vertical or horizontal orientation.
     */
    explicit ScrolledView(orientation orient);

    /**
     * @param[in] parent The parent Frame.
     * @param[in] rect Rectangle for the widget.
     * @param[in] orient Vertical or horizontal orientation.
     */
    explicit ScrolledView(Frame& parent, const Rect& rect = Rect(),
                          orientation orient = orientation::horizontal);

    /**
     * @param[in] parent The parent Frame.
     * @param[in] orient Vertical or horizontal orientation.
     */
    explicit ScrolledView(Frame& parent, orientation orient);

    virtual std::unique_ptr<Widget> clone() override
    {
        return std::unique_ptr<Widget>(make_unique<ScrolledView>(*this).release());
    }

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

    virtual ~ScrolledView() noexcept = default;

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

    /**
     * Starting offset for the drag.
     */
    int m_start_offset{0};
};

}
}

#endif
