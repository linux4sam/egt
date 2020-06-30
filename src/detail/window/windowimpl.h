/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_WINDOW_WINDOWIMPL_H
#define EGT_DETAIL_WINDOW_WINDOWIMPL_H

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

    WindowImpl() = delete;

    /**
     * Construct the implementation and provide a pointer to the parent
     * interface.
     */
    explicit WindowImpl(Window* inter, Screen* screen = nullptr);

    virtual void damage(const Rect& rect);
    virtual Screen* screen();
    virtual void resize(const Size& size);
    virtual void scale(float scalex, float scaley);
    virtual void move(const Point& point);
    virtual void begin_draw();
    virtual void show();
    virtual void hide();
    virtual void paint(Painter& painter);
    virtual void allocate_screen() {}
    inline bool has_screen() const
    {
        return m_screen != nullptr;
    }

    virtual ~WindowImpl() = default;

protected:

    Screen* m_screen{nullptr};
    Window* m_interface;
};
}

}
}

#endif
