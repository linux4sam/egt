/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/detail/basicwindow.h"
#include "egt/detail/planewindow.h"
#include "egt/painter.h"
#include "egt/kmsscreen.h"
#include "egt/window.h"
#include <algorithm>
#include <sstream>
#include <vector>

using namespace std;

namespace egt
{
inline namespace v1
{

static std::vector<Window*> the_windows;
static Window* the_main_window = nullptr;
static Window* the_modal_window = nullptr;

Window*& main_window()
{
    return the_main_window;
}

Window*& modal_window()
{
    return the_modal_window;
}

std::vector<Window*>& windows()
{
    return the_windows;
}

Window::Window(const Rect& rect,
               widgetmask flags,
               pixel_format format,
               bool heo)
    : Frame(rect, flags | widgetmask::WINDOW)
{
    static auto window_id = 0;
    ostringstream ss;
    ss << "Window" << window_id++;
    set_name(ss.str());

    // by default, windows are hidden
    m_visible = false;

    // create_impl the window implementation
    create_impl(box(), format, heo);

    // save off the new window to the window list
    windows().push_back(this);
}

void Window::do_draw()
{
    if (unlikely(m_damage.empty()))
        return;

    DBG(name() << " " << __PRETTY_FUNCTION__);

    Painter painter(screen()->context());

    for (auto& damage : m_damage)
    {
        draw(painter, damage);
    }

    screen()->flip(m_damage);
    m_damage.clear();
}

void Window::resize(const Size& size)
{
    // cannot resize if we are screen
    if (unlikely(the_main_window == this))
        return;

    m_impl->resize(size);
}

void Window::create_impl(const Rect& rect,
                         pixel_format format,
                         bool heo)
{
    ignoreparam(format);
    ignoreparam(heo);

    assert(main_screen());

    if (!the_main_window)
    {
        the_main_window = this;

        m_box.x = 0;
        m_box.y = 0;
        m_box.w = main_screen()->size().w;
        m_box.h = main_screen()->size().h;

        m_impl.reset(new detail::BasicTopWindow(this));
    }
    else
    {
        m_box = rect;

#ifdef HAVE_LIBPLANES
        static uint32_t count = 0;
        auto s = dynamic_cast<KMSScreen*>(main_screen());
        if (count++ < s->count_planes(KMSScreen::plane_type::overlay))
        {
            DBG(name() << " backend is PlaneWindow");
            m_impl.reset(new detail::PlaneWindow(this, format, heo));
            flag_set(widgetmask::PLANE_WINDOW);
        }
        else
#endif
        {
            DBG(name() << " backend is BasicWindow");
            m_impl.reset(new detail::BasicWindow(this));
        }
    }

    assert(m_impl.get());
}

void Window::set_main_window()
{
    the_main_window = this;

    assert(main_screen());

    m_box.x = 0;
    m_box.y = 0;
    m_box.w = main_screen()->size().w;
    m_box.h = main_screen()->size().h;

    damage();
}

Window::~Window()
{
    auto i = find(the_windows.begin(), the_windows.end(), this);
    if (i != the_windows.end())
    {
        the_windows.erase(i);
    }

    if (the_main_window == this)
    {
        the_main_window = nullptr;

        for (auto& window : the_windows)
        {
            window->set_main_window();
            break;
        }
    }
}

}
}
