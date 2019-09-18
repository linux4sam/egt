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

#include <egt/frame.h>
#include <egt/image.h>
#include <egt/label.h>
#include <egt/screen.h>
#include <iosfwd>
#include <memory>
#include <vector>

namespace egt
{
inline namespace v1
{

/**
 * Hint used for configuring Window backends.
 */
enum class windowhint
{
    /**
     * Allow automatic detection of the window type to create.
     */
    automatic,

    /**
     * Request a software only implementation.
     */
    software,

    /**
     * Request an overlay plane.
     */
    overlay,

    /**
     * Request specifically an HEO overlay plane.
     */
    heo_overlay,

    /**
     * Request a cursor overlay plane.
     */
    cursor_overlay,
};

std::ostream& operator<<(std::ostream& os, const windowhint& event);

namespace detail
{
class WindowImpl;
class PlaneWindow;
}

/**
 * A Window is a Widget that handles drawing to a Screen.
 *
 * A Window is a Frame that optionally manages and draws to a Screen. If the
 * Window does not have a Screen itself, it will refer to its parent for the
 * Screen.
 *
 * Windows, unlike other basic widgets, are hidden by default. Windows always
 * require a call to show() before they will be drawn.
 *
 * This class acts as normal Frame/Widget but punts many operations to a
 * dynamically selected backend to work with the screen.
 */
class Window : public Frame
{
public:

    /**
     * Default pixel format used for Window.
     */
    static const pixel_format DEFAULT_FORMAT;

    explicit Window(pixel_format format = DEFAULT_FORMAT,
                    windowhint hint = windowhint::automatic)
        : Window({}, format, hint)
    {}

    explicit Window(const Rect& rect,
                    pixel_format format = DEFAULT_FORMAT,
                    windowhint hint = windowhint::automatic);

    virtual void damage() override
    {
        Frame::damage();
    }

    virtual void damage(const Rect& rect) override;

    /**
     * The buck stops on this call to Widget::screen() with a Window
     * because the Window contains the screen.
     */
    virtual Screen* screen() override;

    virtual bool has_screen() const override;

    virtual void move(const Point& point) override;

    virtual void show() override;

    virtual void hide() override;

    virtual void resize(const Size& size) override;

    virtual void set_scale(float scale) override;

    virtual void paint(Painter& painter) override;

    /*
     * Damage rectangles propagate up the widget tree and stop at a top level
     * widget, which can only be a window. As it propagates up, the damage
     * rectangle origin changes value to respect the current frame.  When
     * drawing those rectangles, as they propagate down the widget hierarchy
     * the opposite change happens to the rectangle origin.
     */
    virtual void top_draw() override;

    /**
     * Perform the actual drawing.  Allocate the Painter and call draw() on each
     * child.
     */
    virtual void do_draw();

    /**
     * Set a background image of the window.
     *
     * The background image will automatically be scaled to fit the size of the
     * window.
     *
     * @note This is not a special child widget, in the sense that you can
     * technically put another child widget before the background using
     * zorder.
     */
    virtual void set_background(const Image& image);

    virtual ~Window();

protected:

    virtual void allocate_screen();

    /**
     * Select and allocate the backend implementation for the window.
     */
    void create_impl(const Rect& rect,
                     pixel_format format,
                     windowhint hint);

    virtual void default_damage(const Rect& rect)
    {
        Frame::damage(rect);
    }

    virtual void default_resize(const Size& size)
    {
        Frame::resize(size);
    }

    virtual void default_set_scale(float scale)
    {
        Frame::set_scale(scale);
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

    /**
     * Optional background image.
     */
    std::shared_ptr<ImageLabel> m_background;

    friend class detail::WindowImpl;
    friend class detail::PlaneWindow;
};

/**
 * Top level Window.
 *
 * Special Window type that is the top level Window, or main Window.  It
 * provides features like a pointer cursor, that usually only make sense as a
 * top level Window.
 */
class TopWindow : public Window
{
public:

    using Window::Window;

    /**
     * Show the cursor.
     */
    virtual void show_cursor(const Image& image = Image("@cursor.png"));

    /**
     * Hide the cursor.
     */
    virtual void hide_cursor();

protected:

    /**
     * Handle mouse events.
     */
    virtual void handle_mouse(Event& event);

    /**
     * Cursor window, if created.
     */
    std::shared_ptr<Window> m_cursor;
};

/**
 * Get a pointer reference to the main window.
 */
Window* main_window();

/**
 * Get a pointer reference to the modal window.
 *
 * The modal window is a single window that will receive all events. Only
 * one window can be modal at any given time.
 */
Window* modal_window();

namespace detail
{

/**
 * Set the modal window.
 */
void set_modal_window(Window* window);

}

/**
 * Get the list of all currently allocated Windows.
 */
std::vector<Window*>& windows();

}
}

#endif
