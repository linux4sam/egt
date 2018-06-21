/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifdef HAVE_X11

#include "event_loop.h"
#include "input.h"
#include "widget.h"
#include "window.h"
#include "x11screen.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cairo-xlib.h>
#include <cairo.h>
#include <X11/Xlib.h>

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
	: m_priv(new X11Data)
    {
	m_priv->display = XOpenDisplay(NULL);
	assert(m_priv->display);

	EventLoop::add_fd(ConnectionNumber(m_priv->display), EVENT_READABLE,
			  X11Screen::process, this);

	int screen = DefaultScreen(m_priv->display);
	Window win = RootWindow(m_priv->display, screen);

	m_priv->window = XCreateSimpleWindow(m_priv->display,
				       win,
				       0, 0,
				       size.w, size.h,
				       0, 0, 0);

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

	init(0, size.w, size.h);

	XMapWindow(m_priv->display, m_priv->window);

	m_surface_back = shared_cairo_surface_t(cairo_xlib_surface_create(m_priv->display, m_priv->window,
									  DefaultVisual(m_priv->display, screen),
									  size.w, size.h),
						cairo_surface_destroy);
	cairo_xlib_surface_set_size(m_surface_back.get(), size.w, size.h);

	m_cr_back = shared_cairo_t(cairo_create(m_surface_back.get()), cairo_destroy);
	assert(m_cr_back);

	m_priv->wmDeleteMessage = XInternAtom(m_priv->display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(m_priv->display, m_priv->window, &m_priv->wmDeleteMessage, 1);

	XFlush(m_priv->display);
    }

    void X11Screen::flip(const vector<Rect>& damage)
    {
	IScreen::flip(damage);
	XFlush(m_priv->display);
    }

    void X11Screen::process(int fd, uint32_t mask, void *data)
    {
	X11Screen* screen = reinterpret_cast<X11Screen*>(data);
	assert(screen);

	for (;;)
	{
	    if (!XPending(screen->m_priv->display))
		break;

	    XEvent e;
	    XNextEvent(screen->m_priv->display, &e);

	    switch (e.type)
	    {
	    case MapNotify:
	    case ConfigureNotify:
	    case Expose:
	    {
		vector<Rect> damage;
		damage.push_back(Rect(0,0,screen->size().w, screen->size().h));
		screen->flip(damage);
		cairo_surface_flush(screen->m_surface_back.get());
		break;
	    }
	    case ButtonPress:
		mouse_position() = Point(e.xbutton.x,e.xbutton.y);
		main_window()->handle(EVT_MOUSE_DOWN);
		break;
	    case ButtonRelease:
		mouse_position() = Point(e.xbutton.x,e.xbutton.y);
		main_window()->handle(EVT_MOUSE_UP);
		break;
	    case EnterNotify:
	    case LeaveNotify:
	    case MotionNotify:
		mouse_position() = Point(e.xbutton.x,e.xbutton.y);
		main_window()->handle(EVT_MOUSE_MOVE);
		break;
	    case KeyPress:
	    {
		//char keybuf[8];
		//KeySym key;
		//XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
		//cout << keybuf << endl;

		key_position() = XLookupKeysym(&e.xkey,0);
		main_window()->handle(EVT_KEYDOWN);
		break;
	    }
	    case KeyRelease:
	    {
		//char keybuf[8];
		//KeySym key;

		//XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
		//cout << keybuf << endl;

		key_position() = XLookupKeysym(&e.xkey,0);
		main_window()->handle(EVT_KEYUP);
		break;
	    }
	    case ClientMessage:
		if (e.xclient.data.l[0] == screen->m_priv->wmDeleteMessage)
		    EventLoop::quit();
		break;
	    default:
		dbg << "x11 event unhandled " << e.type << endl;
		break;
	    }
	}
    }

    X11Screen::~X11Screen()
    {
	EventLoop::rem_fd(ConnectionNumber(m_priv->display));
	XCloseDisplay(m_priv->display);
    }

}

#endif
