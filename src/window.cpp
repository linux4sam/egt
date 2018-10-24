/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/kmsscreen.h"
#include "mui/window.h"
#include <algorithm>

using namespace std;

namespace mui
{
    static std::vector<Window*> the_windows;
    static Window* the_window = nullptr;

    Window*& main_window()
    {
        return the_window;
    }

    std::vector<Window*>& windows()
    {
        return the_windows;
    }

    Window::Window(const Size& size, widgetmask flags)
        : Frame(Rect(size), flags | widgetmask::WINDOW)
    {
        DBG("new window " << size);
        windows().push_back(this);

        // if a window size is empty, or this window is the first window, it
        // must be the size of the screen
        if (!the_window || size.empty())
        {
            assert(main_screen());

            m_box.w = main_screen()->size().w;
            m_box.h = main_screen()->size().h;
        }

        if (!the_window)
        {
            the_window = this;
        }

        hide();
        //damage();

        // go ahead and pick up the default screen
        m_screen = main_screen();
    }

    void Window::show()
    {
        for (auto& child : m_children)
        {
            if (child->align() != alignmask::NONE)
            {
                auto r = align_algorithm(child->size(), child->parent()->box(), child->align(), child->margin());
                child->set_box(r);
            }
        }

        Widget::show();
    }

#ifdef HAVE_LIBPLANES
    PlaneWindow::PlaneWindow(const Size& size, widgetmask flags,
                             uint32_t format, bool heo)
        : Window(size, flags | widgetmask::PLANE_WINDOW),
          m_format(format),
          m_heo(heo)
    {
        // default plane windows to transparent
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        assert(KMSScreen::instance());

        if (!size.empty())
        {
            do_resize(size);
        }
    }

    void PlaneWindow::resize(const Size& size)
    {
        do_resize(size);
    }

    void PlaneWindow::do_resize(const Size& size)
    {
        // severe limitation that we can only resize once
        // this is horrible: basically, only let the plane get allocated once,
        // but it starts at main_screen()
        if (m_screen == main_screen())
        {
            m_screen = new KMSOverlayScreen(
                KMSScreen::instance()->allocate_overlay(size, m_format, m_heo));
            assert(m_screen);
            m_box.size(size);
            damage();
        }
        else
        {
            ERR("plane window only supports single call to resize");
        }
    }

    void PlaneWindow::move(const Point& point)
    {
        if (point != box().point())
        {
            KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
            screen->position(point);

            m_box.point(point);

            m_dirty = true;
        }
    }

    // damage to a plane window does not propagate up, unlike a normal frame
    void PlaneWindow::damage(const Rect& rect)
    {
        if (rect.empty())
            return;

        m_dirty = true;

        add_damage(rect);
    }

    void PlaneWindow::draw()
    {
        if (m_dirty)
        {
            KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(m_screen);
            if (s)
            {
                s->apply();
                m_dirty = false;
            }
        }

        Window::do_draw();
    }

    void PlaneWindow::show()
    {
        KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(m_screen);
        if (s)
        {
            s->show();
            m_dirty = true;
        }

        Window::show();
    }

    void PlaneWindow::hide()
    {
        KMSOverlayScreen* s = reinterpret_cast<KMSOverlayScreen*>(m_screen);
        if (s)
        {
            s->hide();
            m_dirty = true;
        }

        Window::hide();
        /** TODO: no way to hide. */
    }
#else
    PlaneWindow::PlaneWindow(const Size& size, widgetmask flags, uint32_t format, bool heo)
        : Window(size, flags | widgetmask::WINDOW)
    {
        ignoreparam(format);
        ignoreparam(heo);
    }

    void PlaneWindow::damage(const Rect& rect)
    {
        Window::damage(rect);
    }

    void PlaneWindow::resize(const Size& size)
    {
        ignoreparam(size);
    }

    void PlaneWindow::move(const Point& point)
    {
        Window::move(point);
    }

    void PlaneWindow::draw()
    {
        Window::draw();
    }

    void PlaneWindow::show()
    {
        Window::show();
    }

    void PlaneWindow::hide()
    {
        Window::hide();
    }
#endif

    PlaneWindow::~PlaneWindow()
    {
        /** @todo Need to release plane. */
    }

}
