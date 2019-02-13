/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_WINDOW_H
#define EGT_WINDOW_H

/**
 * @file
 * @brief Working with windows.
 */

#include <egt/detail/windowimpl.h>
#include <egt/frame.h>
#include <egt/screen.h>
#include <memory>

namespace egt
{
inline namespace v1
{

namespace detail
{
class BasicWindow;
class BasicTopWindow;
class PlaneWindow;
}

class Window;

/**
 * Get a pointer reference to the main window.
 */
Window*& main_window();

/**
 * Get a pointer reference to the modal window.
 *
 * The modal window is a single window that will receive all events. Only
 * one window can be modal at any given time.
 */
Window*& modal_window();

/**
 * Get the list of all currently allocated BasicWindows.
 */
std::vector<Window*>& windows();

/**
 * Window interface.
 *
 * A Window is a Frame that manages and draws to a Screen.
 *
 * This class acts as normal Frame/Widget put punts many operations to a
 * dynamically selected backend to work with the screen.
 */
class Window : public Frame
{
public:
    constexpr static const auto DEFAULT_FORMAT = pixel_format::argb8888;

    Window(const Rect& rect,
           const widgetflags& flags = widgetflags(),
           pixel_format format = DEFAULT_FORMAT, bool heo = false);

    Window(const Size& size = Size(),
           const widgetflags& flags = widgetflags(),
           pixel_format format = DEFAULT_FORMAT, bool heo = false)
        : Window(Rect(Point(), size), flags, format, heo)
    {}

    virtual void damage() override
    {
        Frame::damage();
    }

    virtual void damage(const Rect& rect) override
    {
        m_impl->damage(rect);
    }

    /**
     * The buck stops on this call to Widget::screen() with a Window
     * because the Window contains the screen.
     */
    virtual IScreen* screen() override
    {
        return m_impl->screen();
    }

    virtual void move(const Point& point) override
    {
        m_impl->move(point);
    }

    virtual void show() override
    {
        m_impl->show();
    }

    virtual void hide() override
    {
        m_impl->hide();
    }

    virtual void resize(const Size& size) override;

    virtual void paint(Painter& painter) override
    {
        m_impl->paint(painter);
    }

    /*
     * Damage rectangles propogate up the widget tree and stop at a top level
     * widget, which can only be a window. As it propogates up, the damage
     * rectangle origin changes value to respect the current frame.  When
     * drawing those rectangles, as they propogate down the widget hierarchy
     * the opposite change happens to the rectangle origin.
     */
    virtual void top_draw() override
    {
        m_impl->top_draw();
    }

    /**
     * Perform the actual drawing.  Allocate the Painter and call draw() on each
     * child.
     */
    virtual void do_draw();

    virtual ~Window();

protected:

    virtual void allocate_screen()
    {
        m_impl->allocate_screen();
    }

    /**
     * Select and allocate the backend implementation for the window.
     */
    void create_impl(const Rect& rect,
                     pixel_format format = pixel_format::argb8888,
                     bool heo = false);

    virtual void default_damage(const Rect& rect)
    {
        Frame::damage(rect);
    }

    virtual void default_resize(const Size& size)
    {
        Frame::resize(size);
    }

    virtual void default_move(const Point& point)
    {
        Frame::move(point);
    }

    virtual void default_top_draw()
    {
        if (m_parent)
        {
            m_parent->top_draw();
            return;
        }

        do_draw();
    }

    virtual void default_show()
    {
        Frame::show();
    }

    virtual void default_hide()
    {
        Frame::hide();
    }

    virtual void default_paint(Painter& painter)
    {
        Frame::paint(painter);
    }

    std::unique_ptr<detail::WindowImpl> m_impl;

    /**
     * Change this window as the main window.
     */
    void set_main_window();

    friend class detail::WindowImpl;
    friend class detail::PlaneWindow;
    friend class detail::BasicTopWindow;
};

using TopWindow = Window;

}
}

#endif
