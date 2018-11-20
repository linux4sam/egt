/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/kmsscreen.h"
#include "egt/painter.h"
#include "egt/planewindow.h"
#include <algorithm>

using namespace std;

namespace egt
{
    static auto planewindow_id = 0;

#ifdef HAVE_LIBPLANES
    PlaneWindow::PlaneWindow(const Size& size, widgetmask flags,
                             uint32_t format, bool heo)
        : Window(size, flags | widgetmask::PLANE_WINDOW),
          m_format(format),
          m_heo(heo)
    {
        assert(KMSScreen::instance());

        ostringstream ss;
        ss << "planewindow" << planewindow_id++;
        set_name(ss.str());

        // default plane windows to transparent
        //palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        do_resize(size);

        m_screen = nullptr;
    }

    PlaneWindow::PlaneWindow(const Rect& rect, widgetmask flags,
                             uint32_t format, bool heo)
        : Window(rect.size(), flags | widgetmask::PLANE_WINDOW),
          m_format(format),
          m_heo(heo)
    {
        assert(KMSScreen::instance());

        ostringstream ss;
        ss << "planewindow" << planewindow_id++;
        set_name(ss.str());

        // default plane windows to transparent
        //palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        m_box = rect;
        m_dirty = true;

        m_screen = nullptr;
    }

    void PlaneWindow::do_resize(const Size& size)
    {
        if (!size.empty())
        {
            m_box.size(size);
            m_dirty = true;
            damage(box());
        }
    }

    void PlaneWindow::move(const Point& point)
    {
        if (point != box().point())
        {
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

    void PlaneWindow::allocate_screen()
    {
        if (!m_screen)
        {
            m_screen = new KMSOverlayScreen(
                KMSScreen::instance()->allocate_overlay(box().size(), m_format, m_heo));
        }
    }

    void PlaneWindow::draw()
    {
        if (!m_box.size().empty())
        {
            if (m_dirty)
            {
                allocate_screen();

                if (visible())
                {
                    KMSOverlayScreen* s = dynamic_cast<KMSOverlayScreen*>(m_screen);
                    assert(s);
                    if (s)
                    {
                        s->position(box().point());
                        s->apply();
                        m_dirty = false;
                    }
                }
            }

            Window::do_draw();
        }
    }

    void PlaneWindow::paint(Painter& painter)
    {
        Painter::AutoSaveRestore sr(painter);

        auto surface = cairo_get_target(screen()->context().get());
        painter.draw_image(point(), surface);
    }

    void PlaneWindow::show()
    {
        m_dirty = true;
        Window::show();
    }

    void PlaneWindow::hide()
    {
        KMSOverlayScreen* s = dynamic_cast<KMSOverlayScreen*>(m_screen);
        if (s)
        {
            s->hide();
            m_dirty = false;
        }

        Window::hide();
    }
#else
    PlaneWindow::PlaneWindow(const Size& size, widgetmask flags, uint32_t format, bool heo)
        : Window(size, flags | widgetmask::WINDOW)
    {
        ignoreparam(format);
        ignoreparam(heo);

        ostringstream ss;
        ss << "planewindow" << planewindow_id++;
        set_name(ss.str());
    }

    PlaneWindow::PlaneWindow(const Rect& rect, widgetmask flags,
                             uint32_t format, bool heo)
        : PlaneWindow(rect.size(), flags, format, heo)
    {
        m_box = rect;
    }

    void PlaneWindow::damage(const Rect& rect)
    {
        Window::damage(rect);
    }

    void PlaneWindow::move(const Point& point)
    {
        Window::move(point);
    }

    void PlaneWindow::draw()
    {
        Window::draw();
    }

    void PlaneWindow::paint(Painter& painter)
    {
        Window::paint(painter);
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
