/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "detail/input/inputkeyboard.h"
#include "detail/screen/keyboard_code_conversion_x.h"
#include "detail/screen/x11wrap.h"
#include "egt/app.h"
#include "egt/detail/screen/x11screen.h"
#include "egt/eventloop.h"
#include "egt/input.h"
#include "egt/keycode.h"
#include <cairo-xlib.h>
#include <cairo.h>
#include <cstdlib>
#include <string>

namespace egt
{
inline namespace v1
{
namespace detail
{

struct X11Data
{
    Display* display{};
    Drawable window{};
    Atom wmDeleteMessage{};

    /// Keyboard instance
    InputKeyboard keyboard;
};

X11Screen::X11Screen(Application& app, const Size& size, const std::string& name, bool borderless)
    : m_app(app),
      m_priv(std::make_unique<detail::X11Data>()),
      m_input(m_app.event().io())
{
    detail::info("X11 Screen");

    m_priv->display = XOpenDisplay(nullptr);
    if (!m_priv->display)
        throw std::runtime_error("unable to connect to X11 display");

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    m_input.assign(ConnectionNumber(m_priv->display));

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    auto screen = DefaultScreen(m_priv->display);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    auto win = RootWindow(m_priv->display, screen);

    m_priv->window = XCreateSimpleWindow(m_priv->display,
                                         win,
                                         0, 0,
                                         size.width(), size.height(),
                                         0, 0, 0);
    if (!m_priv->window)
        throw std::runtime_error("unable to create X11 window");

    if (borderless)
    {
        XSetWindowAttributes winattr;
        winattr.override_redirect = 1;
        XChangeWindowAttributes(m_priv->display, m_priv->window, CWOverrideRedirect, &winattr);
        XSetWindowBorder(m_priv->display, m_priv->window, 0);
    }

    XStoreName(m_priv->display, m_priv->window, name.empty() ? "EGT" : name.c_str());

    auto sh = std::unique_ptr<XSizeHints, decltype(&::XFree)>(XAllocSizeHints(), ::XFree);
    sh->flags = PMinSize | PMaxSize;
    sh->min_width = sh->max_width = size.width();
    sh->min_height = sh->max_height = size.height();
    XSetWMNormalHints(m_priv->display, m_priv->window, sh.get());

    XSelectInput(m_priv->display, m_priv->window,
                 ExposureMask | KeyPressMask | ButtonPressMask |
                 StructureNotifyMask | ButtonReleaseMask |
                 KeyReleaseMask | EnterWindowMask | LeaveWindowMask |
                 PointerMotionMask | Button1MotionMask | VisibilityChangeMask |
                 ColormapChangeMask);

    init(size, PixelFormat::rgb565);

    // instead of using init() to create the buffer, create our own using cairo_xlib_surface_create
    m_buffers.emplace_back(
        cairo_xlib_surface_create(m_priv->display, m_priv->window,
                                  DefaultVisual(m_priv->display, screen), // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
                                  size.width(), size.height()));
    cairo_xlib_surface_set_size(m_buffers.back().surface.get(), size.width(), size.height());

    m_buffers.back().damage.emplace_back(0, 0, size.width(), size.height());

    // remove window decorations
    if (std::getenv("EGT_X11_NODECORATION"))
    {
        disable_window_decorations();
    }

    XMapWindow(m_priv->display, m_priv->window);
    XFlush(m_priv->display);
    XSync(m_priv->display, false);

    m_priv->wmDeleteMessage = XInternAtom(m_priv->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_priv->display, m_priv->window, &m_priv->wmDeleteMessage, 1);

    // start the async read from the server
    asio::async_read(m_input, asio::null_buffers(),
                     [this](const asio::error_code & error, std::size_t)
    {
        handle_read(error);
    });
}

void X11Screen::disable_window_decorations()
{
    struct
    {
        unsigned long flags{}; // NOLINT
        unsigned long functions{}; // NOLINT
        unsigned long decorations{}; // NOLINT
        signed long input_mode{}; // NOLINT
        unsigned long status{}; // NOLINT
    } hints;

    hints.flags = 2;
    hints.decorations = 0;
    Atom property = XInternAtom(m_priv->display, "_MOTIF_WM_HINTS", true);
    XChangeProperty(m_priv->display, m_priv->window,
                    property, property, 32, PropModeReplace,
                    reinterpret_cast<unsigned char*>(&hints), 5);
}

void X11Screen::flip(const DamageArray& damage)
{
    Screen::flip(damage);
    XFlush(m_priv->display);
}

void X11Screen::copy_to_buffer(ScreenBuffer& buffer)
{
    copy_to_buffer_software(buffer);
}

void X11Screen::handle_read(const asio::error_code& error)
{
    if (error)
    {
        detail::error("{}", error);
        return;
    }

    while (XPending(m_priv->display))
    {
        XEvent e;
        XNextEvent(m_priv->display, &e);

        EGTLOG_TRACE("x11 event: {}", e.type);

        switch (e.type)
        {
        default:
        case MapNotify:
        case ConfigureNotify:
            break;
        case Expose:
        {
            DamageArray damage;
            damage.emplace_back(e.xexpose.x, e.xexpose.y,
                                e.xexpose.width, e.xexpose.height);
            flip(damage);
            break;
        }
        case ButtonPress:
        {
            Event event(EventId::raw_pointer_down, Pointer(DisplayPoint(e.xbutton.x, e.xbutton.y)));
            if (e.xbutton.button == Button1)
                event.pointer().btn = Pointer::Button::left;
            else if (e.xbutton.button == Button2)
                event.pointer().btn = Pointer::Button::middle;
            else if (e.xbutton.button == Button3)
                event.pointer().btn = Pointer::Button::right;
            m_in.dispatch(event);
            break;
        }
        case ButtonRelease:
        {
            Event event(EventId::raw_pointer_up, Pointer(DisplayPoint(e.xbutton.x, e.xbutton.y)));
            if (e.xbutton.button == Button1)
                event.pointer().btn = Pointer::Button::left;
            else if (e.xbutton.button == Button2)
                event.pointer().btn = Pointer::Button::middle;
            else if (e.xbutton.button == Button3)
                event.pointer().btn = Pointer::Button::right;
            m_in.dispatch(event);
            break;
        }
        case EnterNotify:
        case LeaveNotify:
        case MotionNotify:
        {
            Event event(EventId::raw_pointer_move, Pointer(DisplayPoint(e.xbutton.x, e.xbutton.y)));
            m_in.dispatch(event);
            break;
        }
        case KeyRelease:
            if (XEventsQueued(m_priv->display, QueuedAfterReading))
            {
                XEvent nev;
                XPeekEvent(m_priv->display, &nev);

                if (nev.type == KeyPress && nev.xkey.time == e.xkey.time &&
                    nev.xkey.keycode == e.xkey.keycode)
                {
                    /* Key wasn’t actually released */
                    XEvent trash;
                    XNextEvent(m_priv->display, &trash);

                    const auto id = EventId::keyboard_repeat;
                    Event event(id, Key(detail::KeyboardCodeFromXKeyEvent(&e),
                                        m_priv->keyboard.on_key(e.xkey.keycode, id),
                                        detail::KeyStateFromXKeyEvent(&e)));
                    m_in.dispatch(event);
                    break;
                }
            }
        //-fallthrough
        case KeyPress:
        {
            const auto id = e.type == KeyPress ? EventId::keyboard_down : EventId::keyboard_up;
            Event event(id, Key(detail::KeyboardCodeFromXKeyEvent(&e),
                                m_priv->keyboard.on_key(e.xkey.keycode, id),
                                detail::KeyStateFromXKeyEvent(&e)));
            m_in.dispatch(event);
            break;
        }
        case ClientMessage:
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
            if (static_cast<int>(e.xclient.data.l[0]) == static_cast<int>(m_priv->wmDeleteMessage))
                m_app.event().quit(0);
            break;
        }
    }

    asio::async_read(m_input, asio::null_buffers(),
                     [this](const asio::error_code & error, std::size_t)
    {
        handle_read(error);
    });
}

X11Screen::~X11Screen() noexcept
{
    if (m_priv->display)
        XCloseDisplay(m_priv->display);
}

}
}
}
