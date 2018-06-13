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

using namespace std;

namespace mui
{

    X11Screen::X11Screen(const Size& size, bool borderless)
    {
	m_display = XOpenDisplay(NULL);
	assert(m_display);

	EventLoop::add_fd(ConnectionNumber(m_display), EVENT_READABLE,
			  X11Screen::process, this);

	int screen = DefaultScreen(m_display);
	Window win = RootWindow(m_display, screen);

	m_window = XCreateSimpleWindow(m_display,
				       win,
				       0, 0,
				       size.w, size.h,
				       0, 0, 0);

	if (borderless)
	{
	    XSetWindowAttributes winattr;
	    winattr.override_redirect = 1;
	    XChangeWindowAttributes(m_display, m_window, CWOverrideRedirect, &winattr);
	    XSetWindowBorder(m_display, m_window, 0);
	}

	XStoreName(m_display, m_window, "ui");

	XSizeHints* sh = XAllocSizeHints();//std::unique_ptr<XSizeHints, decltype(&::Xfree)>(XAllocSizeHints(), ::XFree);
	sh->flags = PMinSize | PMaxSize;
	sh->min_width = sh->max_width = size.w;
	sh->min_height = sh->max_height = size.h;
	XSetWMNormalHints(m_display, m_window, sh);
	XFree(sh);

	XSelectInput(m_display, m_window,
		     ExposureMask | KeyPressMask | ButtonPress |
		     StructureNotifyMask | ButtonReleaseMask |
		     KeyReleaseMask | EnterWindowMask | LeaveWindowMask |
		     PointerMotionMask | Button1MotionMask | VisibilityChangeMask |
		     ColormapChangeMask);

	init(0, size.w, size.h);

	XMapWindow(m_display, m_window);

	m_surface_back = shared_cairo_surface_t(cairo_xlib_surface_create(m_display, m_window,
									  DefaultVisual(m_display, screen),
									  size.w, size.h),
						cairo_surface_destroy);
	cairo_xlib_surface_set_size(m_surface_back.get(), size.w, size.h);

	m_cr_back = shared_cairo_t(cairo_create(m_surface_back.get()), cairo_destroy);
	assert(m_cr_back);

	XFlush(m_display);
    }

    void X11Screen::flip(const vector<Rect>& damage)
    {
	IScreen::flip(damage);
	XFlush(m_display);
    }

    void X11Screen::process(int fd, uint32_t mask, void *data)
    {
	X11Screen* screen = reinterpret_cast<X11Screen*>(data);
	assert(screen);

	for (;;)
	{
	    if (!XPending(screen->m_display))
		break;

	    XEvent e;
	    XNextEvent(screen->m_display, &e);

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
	    default:
		dbg << "x11 event unhandled " << e.type << endl;
		break;
	    }
	}
    }

    X11Screen::~X11Screen()
    {
	EventLoop::rem_fd(ConnectionNumber(m_display));
	XCloseDisplay(m_display);
    }

}

#endif
