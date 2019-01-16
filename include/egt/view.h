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

namespace egt
{
inline namespace v1
{

/**
 * A ScrolledView is a sort of kitchen window. You can have a large surface
 * area on the other side of the window, but you only see a small portion of
 * it through the window.  The surface can be scrolled, or panned, to see
 * the rest.
 */
class ScrolledView : public Frame
{
public:
    enum class Orientation
    {
        HORIZONTAL,
        VERTICAL
    };

    explicit ScrolledView(const Rect& rect = Rect());

    virtual int handle(eventid event) override;

    virtual void draw(Painter& painter, const Rect& rect) override;

    /**
     * Get the position.
     */
    int position() const { return m_offset; }

    /**
     * Set the position.
     */
    void set_position(int offset)
    {
        Rect super = super_rect();

        if (offset <= 0 && -offset < super.w)
        {
            if (m_offset != offset)
            {
                m_offset = offset;
                damage();
            }
        }
    }

    virtual ~ScrolledView();

protected:

    Rect super_rect() const;

    int m_offset{0};
    Orientation m_orientation{Orientation::HORIZONTAL};
    bool m_moving{false};
    Point m_start_pos;
    int m_start_offset{0};
};

}
}

#endif
