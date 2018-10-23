/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_X11

#include "mui/app.h"
#include "mui/event_loop.h"
#include "mui/input.h"
#include "mui/widget.h"
#include "mui/x11screen.h"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cairo-xlib.h>
#include <cairo.h>

using namespace std;

namespace mui
{
    struct X11Data
    {
        Display* display;
        Drawable window;
        Atom wmDeleteMessage;
    };

    X11Screen::X11Screen(const Size& size, bool borderless)
        : m_priv(new X11Data),
          m_input(main_app().event().io())
    {
        m_priv->display = XOpenDisplay(NULL);
        assert(m_priv->display);

        m_input.assign(ConnectionNumber(m_priv->display));

        int screen = DefaultScreen(m_priv->display);
        Window win = RootWindow(m_priv->display, screen);

        m_priv->window = XCreateSimpleWindow(m_priv->display,
                                             win,
                                             0, 0,
                                             size.w, size.h,
                                             0, 0, 0);
        assert(m_priv->window);

        if (borderless)
        {
            XSetWindowAttributes winattr;
            winattr.override_redirect = 1;
            XChangeWindowAttributes(m_priv->display, m_priv->window, CWOverrideRedirect, &winattr);
            XSetWindowBorder(m_priv->display, m_priv->window, 0);
        }

        XStoreName(m_priv->display, m_priv->window, "ui");

        XSizeHints* sh = XAllocSizeHints();//std::unique_ptr<XSizeHints, decltype(&::Xfree)>(XAllocSizeHints(), ::XFree);
        sh->flags = PMinSize | PMaxSize;
        sh->min_width = sh->max_width = size.w;
        sh->min_height = sh->max_height = size.h;
        XSetWMNormalHints(m_priv->display, m_priv->window, sh);
        XFree(sh);

        XSelectInput(m_priv->display, m_priv->window,
                     ExposureMask | KeyPressMask | ButtonPress |
                     StructureNotifyMask | ButtonReleaseMask |
                     KeyReleaseMask | EnterWindowMask | LeaveWindowMask |
                     PointerMotionMask | Button1MotionMask | VisibilityChangeMask |
                     ColormapChangeMask);

        init(nullptr, 0, size.w, size.h);

        // instead of using init() to create the buffer, create our own using cairo_xlib_surface_create
        DisplayBuffer buffer;
        buffer.surface =
            shared_cairo_surface_t(cairo_xlib_surface_create(m_priv->display, m_priv->window,
                                   DefaultVisual(m_priv->display, screen),
                                   size.w, size.h),
                                   cairo_surface_destroy);
        cairo_xlib_surface_set_size(buffer.surface.get(), size.w, size.h);

        buffer.cr = shared_cairo_t(cairo_create(buffer.surface.get()), cairo_destroy);
        buffer.damage.emplace_back(0, 0, size.w, size.h);
        m_buffers.push_back(buffer);

        XMapWindow(m_priv->display, m_priv->window);
        XFlush(m_priv->display);
        XSync(m_priv->display, false);

        m_priv->wmDeleteMessage = XInternAtom(m_priv->display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(m_priv->display, m_priv->window, &m_priv->wmDeleteMessage, 1);

        // start the async read from the server
        asio::async_read(m_input, asio::null_buffers(),
                         std::bind(&X11Screen::handle_read, this, std::placeholders::_1));
    }

    void X11Screen::flip(const damage_array& damage)
    {
        IScreen::flip(damage);
        XFlush(m_priv->display);
        XSync(m_priv->display, false);
    }

    void X11Screen::handle_read(const asio::error_code& error)
    {
        if (error)
        {
            ERR(error);
            return;
        }

        while (XPending(m_priv->display))
        {
            XEvent e;
            XNextEvent(m_priv->display, &e);

            DBG("x11 event: " << e.type);

            switch (e.type)
            {
            case MapNotify:
                break;
            case ConfigureNotify:
                break;
            case Expose:
            {
                damage_array damage;
                damage.emplace_back(e.xexpose.x, e.xexpose.y,
                                    e.xexpose.width, e.xexpose.height);
                flip(damage);
                break;
            }
            case ButtonPress:
                mouse_position() = Point(e.xbutton.x, e.xbutton.y);
                IInput::dispatch(EVT_MOUSE_DOWN);
                break;
            case ButtonRelease:
                mouse_position() = Point(e.xbutton.x, e.xbutton.y);
                IInput::dispatch(EVT_MOUSE_UP);
                break;
            case EnterNotify:
            case LeaveNotify:
            case MotionNotify:
                mouse_position() = Point(e.xbutton.x, e.xbutton.y);
                IInput::dispatch(EVT_MOUSE_MOVE);
                break;
            case KeyPress:
            {
                //char keybuf[8];
                //KeySym key;
                //XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
                //cout << keybuf << endl;

                key_value() = XLookupKeysym(&e.xkey, 0);
                IInput::dispatch(EVT_KEY_DOWN);
                break;
            }
            case KeyRelease:
            {
                //char keybuf[8];
                //KeySym key;
                //XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
                //cout << keybuf << endl;

                key_value() = XLookupKeysym(&e.xkey, 0);
                IInput::dispatch(EVT_KEY_UP);
                break;
            }
            case ClientMessage:
                if ((int)e.xclient.data.l[0] == (int)m_priv->wmDeleteMessage)
                    main_app().event().quit();
                break;
            default:
                DBG("x11 unhandled event: " << e.type);
                break;
            }
        }

        asio::async_read(m_input, asio::null_buffers(),
                         std::bind(&X11Screen::handle_read, this, std::placeholders::_1));
    }

    X11Screen::~X11Screen()
    {
        XCloseDisplay(m_priv->display);
    }

}

#endif
