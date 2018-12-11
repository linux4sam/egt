/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/image.h"
#include "egt/kmsscreen.h"
#include "egt/painter.h"
#include "egt/planewindow.h"
#include <algorithm>

using namespace std;

namespace egt
{

#ifdef HAVE_LIBPLANES
    static auto planewindow_id = 0;

    Window::Window(const Size& size, widgetmask flags,
                             pixel_format format, bool heo)
        : BasicWindow(size, flags | widgetmask::PLANE_WINDOW),
          m_format(format),
          m_heo(heo)
    {
        assert(KMSScreen::instance());

        ostringstream ss;
        ss << "planewindow" << planewindow_id++;
        set_name(ss.str());

        do_resize(size);

        m_screen = nullptr;
    }

    Window::Window(const Rect& rect, widgetmask flags,
                             pixel_format format, bool heo)
        : BasicWindow(rect.size(), flags | widgetmask::PLANE_WINDOW),
          m_format(format),
          m_heo(heo)
    {
        assert(KMSScreen::instance());

        ostringstream ss;
        ss << "planewindow" << planewindow_id++;
        set_name(ss.str());

        m_box = rect;
        m_dirty = true;

        m_screen = nullptr;
    }

    void Window::do_resize(const Size& size)
    {
        if (!size.empty())
        {
            m_box.size(size);
            m_dirty = true;
            damage(box());
        }
    }

    void Window::move(const Point& point)
    {
        if (point != box().point())
        {
            m_box.point(point);
            m_dirty = true;
        }
    }

    // damage to a plane window does not propagate up, unlike a normal frame
    void Window::damage(const Rect& rect)
    {
        auto crect = Rect(rect.point() - box().point(), rect.size());

        if (crect.empty())
            return;

        // don't damage if not even visible
        if (!visible())
            return;

        m_dirty = true;

        add_damage(crect);
    }

    void Window::allocate_screen()
    {
        if (!m_screen)
        {
            m_screen = new KMSOverlay(
                KMSScreen::instance()->allocate_overlay(box().size(),
                        m_format, m_heo));
        }
    }

    void Window::top_draw()
    {
        if (!m_box.size().empty())
        {
            if (m_dirty)
            {
                allocate_screen();

                if (visible())
                {
                    KMSOverlay* s = dynamic_cast<KMSOverlay*>(m_screen);
                    assert(s);
                    if (s)
                    {
                        s->position(box().point());
                        s->apply();
                        m_dirty = false;
                    }
                }
            }

            BasicWindow::do_draw();
        }
    }

    void Window::paint(Painter& painter)
    {
        Painter::AutoSaveRestore sr(painter);

        auto image = Image(cairo_get_target(screen()->context().get()));
        painter.draw_image(point(), image);
    }

    void Window::show()
    {
        m_dirty = true;
        BasicWindow::show();
    }

    void Window::hide()
    {
        KMSOverlay* s = dynamic_cast<KMSOverlay*>(m_screen);
        if (s)
        {
            s->hide();
            m_dirty = false;
        }

        BasicWindow::hide();
    }
#else
    Window::Window(const Size& size, widgetmask flags, pixel_format format, bool heo)
        : BasicWindow(size, flags)
    {
        ignoreparam(format);
        ignoreparam(heo);
    }

    Window::Window(const Rect& rect, widgetmask flags,
                             pixel_format format, bool heo)
        : Window(rect.size(), flags, format, heo)
    {
        m_box = rect;
    }

    void Window::damage(const Rect& rect)
    {
        BasicWindow::damage(rect);
    }

    void Window::move(const Point& point)
    {
        BasicWindow::move(point);
    }

    void Window::allocate_screen()
    {
    }

    void Window::top_draw()
    {
        BasicWindow::top_draw();
    }

    void Window::paint(Painter& painter)
    {
        BasicWindow::paint(painter);
    }

    void Window::show()
    {
        BasicWindow::show();
    }

    void Window::hide()
    {
        BasicWindow::hide();
    }
#endif

    Window::~Window()
    {
        /** @todo Need to release plane. */
    }

}
