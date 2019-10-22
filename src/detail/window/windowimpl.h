/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_WINDOWIMPL_H
#define EGT_DETAIL_WINDOWIMPL_H

#include <egt/geometry.h>

namespace egt
{
inline namespace v1
{

class Screen;
class Painter;
class Window;

namespace detail
{

/**
 * Defines the interface for a Window backend implementation.
 */
class WindowImpl
{
public:

    /**
     * Construct the implementation and provide a pointer to the parent
     * interface.
     */
    explicit WindowImpl(Window* interface);

    virtual void damage(const Rect& rect);
    virtual Screen* screen();
    virtual void resize(const Size& size);
    virtual void set_scale(float scalex, float scaley);
    virtual void move(const Point& point);
    virtual void top_draw();
    virtual void show();
    virtual void hide();
    virtual void paint(Painter& painter);
    virtual void allocate_screen() {}
    inline bool has_screen() const
    {
        return m_screen != nullptr;
    }

    virtual ~WindowImpl()
    {}

protected:

    Screen* m_screen{nullptr};
    Window* m_interface;

    WindowImpl() = delete;
};
}

}
}

#endif
