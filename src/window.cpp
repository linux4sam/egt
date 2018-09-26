/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "kmsscreen.h"
#include "window.h"
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

    Window::Window(const Size& size, uint32_t flags)
        : Frame(Point(), size, flags)
    {
        cout << "new window " << size << endl;
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

        damage();

        // go ahead and pick up the default screen
        m_screen = main_screen();
    }

#ifdef HAVE_LIBPLANES
    PlaneWindow::PlaneWindow(const Size& size, uint32_t flags, uint32_t format, bool heo)
        : Window(size, flags | FLAG_PLANE_WINDOW),
          m_format(format),
          m_heo(heo)
    {
        // default plane windows to transparent
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        assert(KMSScreen::instance());

        if (!size.empty())
        {
            resize(this->w(), this->h());
        }
    }

    void PlaneWindow::resize(int w, int h)
    {
        // severe limitation that we can only size once
        // this is horrible: basically, only let the plane get allocated once,
        // but it starts at main_screen()
        if (m_screen == main_screen())
        {
            m_screen = new KMSOverlayScreen(
                KMSScreen::instance()->allocate_overlay(Size(w, h), m_format, m_heo));
            assert(m_screen);
            size(w, h);
            damage();
        }
        else
        {
            cout << "plane window only supports single call to resize" << endl;
        }
    }

    void PlaneWindow::position(int x, int y)
    {
        if (x != box().x || y != box().y)
        {
            KMSOverlayScreen* screen = reinterpret_cast<KMSOverlayScreen*>(m_screen);
            screen->position(x, y);

            m_box.x = x;
            m_box.y = y;

            m_dirty = true;
        }
    }

    void PlaneWindow::move(int x, int y)
    {
        position(x, y);
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
        //std::cout << name() << " " << __PRETTY_FUNCTION__ << std::endl;

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
#else
    PlaneWindow::PlaneWindow(const Size& size, uint32_t flags, uint32_t format, bool heo)
        : Window(size, flags),
          m_dirty(true)
    {

    }

    void PlaneWindow::position(int x, int y)
    {
        Window::position(x, y);
    }

    void PlaneWindow::move(int x, int y)
    {
        Window::move(x, y);
    }

    void PlaneWindow::draw()
    {
        Window::draw();
    }
#endif

    PlaneWindow::~PlaneWindow()
    {
    }

}
