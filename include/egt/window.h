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
 * This class acts as a normal Frame/Widget but punts many operations to a
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

    /**
     * Construct a window.
     *
     * @param[in] parent Parent Frame of the Widget.
     * @param[in] rect Initial rectangle of the Window.
     * @param[in] format_hint Requested format of the Window. This only applies
     *            if this Window will be responsible for creating a backing
     *            screen. Otherwise, the Window will use whatever format the
     *            existing screen has. This is only a hint.
     * @param[in] hint Requested Window type. This only applies if this Window
     *            will be responsible for creating a backing screen.  This is
     *            only a hint.
     */
    Window(Frame& parent,
           const Rect& rect,
           PixelFormat format_hint = DEFAULT_FORMAT,
           WindowHint hint = WindowHint::automatic);
    /**
     * Construct a window.
     *
     * @param[in] props list of widget argument and its properties.
     */
    explicit Window(Serializer::Properties& props)
        : Window(props, false)
    {
    }

protected:

    explicit Window(Serializer::Properties& props, bool is_derived);

public:

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) noexcept;
    Window& operator=(Window&&) noexcept;

    using Frame::damage;

    void damage(const Rect& rect) override;

    /**
     * The buck stops on this call to Widget::screen() with a Window
     * because the Window contains the screen.
     */
    EGT_NODISCARD Screen* screen() const override;

    EGT_NODISCARD bool has_screen() const override;

    void move(const Point& point) override;

    void show() override;

    void hide() override;

    void resize(const Size& size) override;

    using Frame::scale;

    void scale(float hscale, float vscale) override;

    /**
     * Get horizontal scale value.
     */
    EGT_NODISCARD float hscale() const
    {
        return m_hscale;
    }

    /**
     * Get vertical scale value.
     */
    EGT_NODISCARD float vscale() const
    {
        return m_vscale;
    }

    /*
     * Damage rectangles propagate up the widget tree and stop at a top level
     * widget, which can only be a window. As it propagates up, the damage
     * rectangle origin changes value to respect the current frame.  When
     * drawing those rectangles, as they propagate down the widget hierarchy
     * the opposite change happens to the rectangle origin.
     */
    void begin_draw() override;
    using Frame::begin_draw;

    /**
     * Get the pixel format of the window.
     */
    EGT_NODISCARD PixelFormat format() const
    {
        auto frame = find_screen();
        if (frame)
            return frame->screen()->format();

        return PixelFormat::invalid;
    }

    /**
     * Set the pixel format hint of the window.
     *
     * The new value is taken into account only by Window::serialize() but
     * has no dynamic effect on the window once created.
     */
    void format_hint(PixelFormat format_hint)
    {
        m_format_hint = format_hint;
    }

    /**
     * Get the pixel format hint of the window.
     *
     * This is the value that would be used if Window::serialize() were called
     * but not necessarily the actuel pixel format of the window: for this
     * later value, call Window::format() instead.
     */
    EGT_NODISCARD PixelFormat format_hint() const { return m_format_hint; }

    /**
     * Set the window hint.
     *
     * The new value is taken into account only by Window::serialize() but
     * has no dynamic effect on the window once created.
     */
    void window_hint(WindowHint hint)
    {
        m_hint = hint;
    }

    /**
     * Get the window hint.
     *
     * This is the value that would be used if Window::serialize() were called
     * but it doesn't necessarily match the actual type of Window::m_impl.
     */
    EGT_NODISCARD WindowHint window_hint() const { return m_hint; }

    void serialize(Serializer& serializer) const override;

    ~Window() noexcept override;

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
            begin_draw(m_parent);
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
    std::unique_ptr<detail::WindowImpl> m_impl;

    /// Set this window as the main window.
    void main_window();

    /// @private
    PixelFormat m_format_hint;

    /// @private
    WindowHint m_hint;

    /// Horizontal scale value.
    float m_hscale{1.0};

    /// Vertical scale value.
    float m_vscale{1.0};

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
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions,hicpp-special-member-functions)
class EGT_API TopWindow : public Window
{
public:

    using Window::Window;

    TopWindow& operator=(const TopWindow&) = delete;
    TopWindow& operator=(TopWindow&&) = default;

    /// Show the cursor.
    void show_cursor(const Image& image = Image("res:internal_cursor"));

    /// Hide the cursor.
    void hide_cursor();

    ~TopWindow() noexcept override;

protected:

    /// Handle mouse events.
    void handle_mouse(Event& event);

    /// Cursor window, if created.
    std::unique_ptr<Window> m_cursor;

private:

    /// Internal registration handle
    Object::RegisterHandle m_handle{0};
};

}
}

#endif
