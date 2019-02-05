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
#include "egt/detail/planewindow.h"
#include <algorithm>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

PlaneWindow::PlaneWindow(Window* interface,
                         pixel_format format, bool heo)
    : BasicWindow(interface),
      m_format(format),
      m_heo(heo)
{
    assert(KMSScreen::instance());
    m_screen = nullptr;
}

void PlaneWindow::resize(const Size& size)
{
    DBG(m_interface->name() << " " << __PRETTY_FUNCTION__);

    if (!size.empty() && m_interface->m_box.size() != size)
    {
        m_interface->m_box.size(size);
        m_dirty = true;
    }
}

void PlaneWindow::move(const Point& point)
{
    DBG(m_interface->name() << " " << __PRETTY_FUNCTION__);

    if (point != m_interface->box().point())
    {
        m_interface->m_box.point(point);
        m_dirty = true;
    }
}

// damage to a plane window does not propagate up, unlike a normal frame
void PlaneWindow::damage(const Rect& rect)
{
    auto crect = Rect(rect.point() - m_interface->box().point(), rect.size());

    if (crect.empty())
        return;

    // don't damage if not even visible
    if (!m_interface->visible())
        return;

    m_dirty = true;

    m_interface->add_damage(crect);
}

void PlaneWindow::allocate_screen()
{
    if (!m_screen)
    {
        m_screen = new KMSOverlay(
            KMSScreen::instance()->allocate_overlay(m_interface->box().size(),
                    m_format, m_heo));
    }
}

void PlaneWindow::top_draw()
{
    if (!m_interface->box().size().empty())
    {
        DBG(m_interface->name() << " " << __PRETTY_FUNCTION__);

        if (m_dirty)
        {
            allocate_screen();

            if (m_interface->visible())
            {
                KMSOverlay* s = dynamic_cast<KMSOverlay*>(m_screen);
                assert(s);
                if (s)
                {
                    s->position(m_interface->to_screen(m_interface->box().point()));
                    s->apply();
                    m_dirty = false;
                }
            }
        }

        m_interface->do_draw();
    }
}

void PlaneWindow::paint(Painter& painter)
{
    Painter::AutoSaveRestore sr(painter);

    auto image = Image(cairo_get_target(screen()->context().get()));
    painter.draw_image(m_interface->point(), image);
}

void PlaneWindow::show()
{
    m_dirty = true;
    BasicWindow::show();
}

void PlaneWindow::hide()
{
    KMSOverlay* s = dynamic_cast<KMSOverlay*>(m_screen);
    if (s)
    {
        s->hide();
        m_dirty = false;
    }

    BasicWindow::hide();
}

PlaneWindow::~PlaneWindow()
{
    /** @todo Need to release plane. */
}

}
}
}
