/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_WINDOWIMPL_H
#define EGT_DETAIL_WINDOWIMPL_H

namespace egt
{
inline namespace v1
{

class IScreen;
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
    virtual IScreen* screen();
    virtual void resize(const Size& size);
    virtual void move(const Point& point);
    virtual void top_draw();
    virtual void show();
    virtual void hide();
    virtual void paint(Painter& painter);
    virtual void allocate_screen() {}

    virtual ~WindowImpl()
    {}

protected:

    IScreen* m_screen{nullptr};
    Window* m_interface;

    WindowImpl() = delete;
};
}

}
}

#endif
