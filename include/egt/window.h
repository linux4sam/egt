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

#include <egt/detail/meta.h>
#include <egt/frame.h>
#include <egt/image.h>
#include <egt/label.h>
#include <egt/screen.h>
#include <egt/widgetflags.h>
#include <memory>
#include <vector>

namespace egt
{
inline namespace v1
{

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
class EGT_API Window : public Frame
{
public:

    /**
     * Default pixel format used for Window.
     */
    static const PixelFormat DEFAULT_FORMAT;

    /**
     * Construct a window.
     *
     * @param[in] format_hint Requested format of the Window. This only applies
     *            if this Window will be responsible for creating a backing
     *            screen. Otherwise, the Window will use whatever format the
     *            existing screen has. This is only a hint.
     * @param[in] hint Requested Window type. This only applies if this Window
     *            will be responsible for creating a backing screen.  This is
     *            only a hint.
     */
    explicit Window(PixelFormat format_hint = DEFAULT_FORMAT,
                    WindowHint hint = WindowHint::automatic)
        : Window({}, format_hint, hint)
    {}

    /**
     * Construct a window.
     *
     * @param[in] rect Initial rectangle of the Window.
     * @param[in] format_hint Requested format of the Window. This only applies
     *            if this Window will be responsible for creating a backing
     *            screen. Otherwise, the Window will use whatever format the
     *            existing screen has. This is only a hint.
     * @param[in] hint Requested Window type. This only applies if this Window
     *            will be responsible for creating a backing screen.  This is
     *            only a hint.
     */
    explicit Window(const Rect& rect,
                    PixelFormat format_hint = DEFAULT_FORMAT,
                    WindowHint hint = WindowHint::automatic);

    EGT_OPS_NOCOPY_MOVE(Window);
    ~Window() override;

    using Frame::damage;

    void damage(const Rect& rect) override;

    /**
     * The buck stops on this call to Widget::screen() with a Window
     * because the Window contains the screen.
     */
    Screen* screen() const override;

    bool has_screen() const override;

    void move(const Point& point) override;

    void show() override;

    void hide() override;

    void resize(const Size& size) override;

    using Frame::scale;

    void scale(float hscale, float vscale) override;

    void paint(Painter& painter) override;

    /*
     * Damage rectangles propagate up the widget tree and stop at a top level
     * widget, which can only be a window. As it propagates up, the damage
     * rectangle origin changes value to respect the current frame.  When
     * drawing those rectangles, as they propagate down the widget hierarchy
     * the opposite change happens to the rectangle origin.
     */
    void begin_draw() override;

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
    virtual void background(const Image& image);

    /**
     * Get the pixel format of the window.
     */
    virtual PixelFormat format() const
    {
        auto frame = find_screen();
        if (frame)
            return frame->screen()->format();

        return PixelFormat::invalid;
    }

protected:

    /**
     * Perform the actual drawing.  Allocate the Painter and call draw() on each
     * child.
     */
    virtual void do_draw();

    /// @private
    virtual void allocate_screen();

    /**
     * Select and allocate the backend implementation for the window.
     */
    void create_impl(const Rect& rect,
                     PixelFormat format_hint,
                     WindowHint hint);

    /// @private
    virtual void default_damage(const Rect& rect)
    {
        Frame::damage(rect);
    }

    /// @private
    virtual void default_resize(const Size& size)
    {
        Frame::resize(size);
    }

    /// @private
    virtual void default_scale(float scalex, float scaley)
    {
        Frame::scale(scalex, scaley);
    }

    /// @private
    virtual void default_move(const Point& point)
    {
        Frame::move(point);
    }

    /// @private
    virtual void default_begin_draw()
    {
        if (m_parent)
        {
            m_parent->begin_draw();
            return;
        }

        do_draw();
    }

    /// @private
    virtual void default_show()
    {
        Frame::show();
    }

    /// @private
    virtual void default_hide()
    {
        Frame::hide();
    }

    /// @private
    virtual void default_paint(Painter& painter)
    {
        Frame::paint(painter);
    }

    /// @private
    std::unique_ptr<detail::WindowImpl> m_impl;

    /// Set this window as the main window.
    void main_window();

    /// Optional background image.
    std::unique_ptr<ImageLabel> m_background;

    friend class detail::WindowImpl;
    friend class detail::PlaneWindow;
};

/**
 * Top level Window.
 *
 * Special Window type that is the top level Window, or MainWindow.  It
 * provides features like a pointer cursor, that usually only make sense as a
 * top level Window.
 */
class EGT_API TopWindow : public Window
{
public:

    using Window::Window;

    EGT_OPS_NOCOPY_MOVE(TopWindow);
    ~TopWindow() override;

    /// Show the cursor.
    virtual void show_cursor(const Image& image = Image("res:internal_cursor"));

    /// Hide the cursor.
    virtual void hide_cursor();

protected:

    /// Handle mouse events.
    virtual void handle_mouse(Event& event);

    /// Cursor window, if created.
    std::unique_ptr<Window> m_cursor;

private:

    /// Internal registration handle
    detail::Object::RegisterHandle m_handle{0};
};

/// Alias for a TopWindow
using MainWindow = TopWindow;

/**
 * Get a pointer to the main window.
 */
EGT_API Window* main_window();

/**
 * Get a pointer to the modal window.
 *
 * The modal window is a single window that will receive all events. Only
 * one window can be modal at any given time.
 */
EGT_API Window* modal_window();

namespace detail
{

/**
 * Set the modal window.
 */
EGT_API void set_modal_window(Window* window);

}

/**
 * Get the list of all currently allocated Windows.
 */
EGT_API std::vector<Window*>& windows();

}
}

#endif
