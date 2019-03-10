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
#include "egt/detail/screen/kmsscreen.h"
#include "egt/input.h"
#include "egt/label.h"
#include "egt/painter.h"
#include "egt/window.h"
#include <algorithm>
#include <iostream>
#include <map>
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

std::ostream& operator<<(std::ostream& os, const windowhint& event)
{
    static std::map<windowhint, std::string> strings;
    if (strings.empty())
    {
#define MAPITEM(p) strings[p] = #p
        MAPITEM(windowhint::automatic);
        MAPITEM(windowhint::software);
        MAPITEM(windowhint::overlay);
        MAPITEM(windowhint::heo_overlay);
        MAPITEM(windowhint::cursor_overlay);
#undef MAPITEM
    }

    os << strings[event];
    return os;
}

Window::Window(const Rect& rect,
               const Widget::flags_type& flags,
               pixel_format format,
               windowhint hint)
    : Frame(rect, flags)
{
    set_name("Window" + std::to_string(m_widgetid));

    ncflags().set(Widget::flag::window);
    set_boxtype(Theme::boxtype::fillsolid);

    // by default, windows are hidden
    ncflags().set(Widget::flag::invisible);

    // create the window implementation
    create_impl(box(), format, hint);

    // save off the new window to the window list
    windows().push_back(this);
}

void Window::do_draw()
{
    if (unlikely(m_damage.empty()))
        return;

    // bookeeping to make sure we don't damage() in draw()
    m_in_draw = true;
    detail::scope_exit reset([this]() { m_in_draw = false; });

    DBG(name() << " " << __PRETTY_FUNCTION__);

    Painter painter(screen()->context());

    for (auto& damage : m_damage)
        draw(painter, damage);

    screen()->flip(m_damage);
    m_damage.clear();
}

void Window::resize(const Size& size)
{
    // cannot resize if we are screen
    if (unlikely(the_main_window == this))
        return;

    if (m_impl)
        m_impl->resize(size);
}

void Window::create_impl(const Rect& rect,
                         pixel_format format,
                         windowhint hint)
{
    ignoreparam(format);
    ignoreparam(hint);

    assert(main_screen());

    if (!the_main_window)
    {
        the_main_window = this;
        m_box = main_screen()->box();
        m_impl.reset(new detail::BasicTopWindow(this));
    }
    else
    {
        m_box = rect;

        try
        {
            switch (hint)
            {
            case windowhint::software:
                m_impl.reset(new detail::BasicWindow(this));
                break;
            case windowhint::overlay:
            case windowhint::heo_overlay:
            case windowhint::cursor_overlay:
#ifdef HAVE_LIBPLANES
                m_impl.reset(new detail::PlaneWindow(this, format, hint));
                flags().set(Widget::flag::plane_window);
#endif
                break;
            default:
                break;
            }
        }
        catch (std::exception& e)
        {
            DBG(e.what());
        }

        if (!m_impl)
        {
#ifdef HAVE_LIBPLANES
            try
            {
                m_impl.reset(new detail::PlaneWindow(this, format, hint));
                flags().set(Widget::flag::plane_window);
            }
            catch (std::exception& e)
            {
                DBG(e.what());
#endif

                m_impl.reset(new detail::BasicWindow(this));
#ifdef HAVE_LIBPLANES
            }
#endif
        }
    }

    assert(m_impl);

    if (flags().is_set(Widget::flag::plane_window))
    {
        DBG(name() << " backend is PlaneWindow");
    }
    else
    {
        DBG(name() << " backend is BasicWindow");
    }
}

void Window::set_main_window()
{
    the_main_window = this;

    assert(main_screen());

    m_box = main_screen()->box();

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

/**
 * @brief Basic implementation of a pointer cursor.
 */
struct CursorWindow : public Window
{
    explicit CursorWindow(const Image& image)
        : Window(image.size(), Widget::flags_type(), pixel_format::argb8888, windowhint::cursor_overlay),
          m_label(new ImageLabel(image))
    {
        palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::transparent);
        set_boxtype(Theme::boxtype::fill);
        m_label->set_boxtype(Theme::boxtype::none);
        add(m_label);
    }

    virtual int handle(eventid) override
    {
        return 0;
    }

    virtual ~CursorWindow() = default;

    std::shared_ptr<ImageLabel> m_label;
};

void TopWindow::hide_cursor()
{
    if (m_cursor)
        m_cursor->hide();
}

void TopWindow::show_cursor(const Image& image)
{
    m_cursor.reset(new CursorWindow(image));
    m_cursor->move(main_screen()->box().center());
    add(m_cursor);
    m_cursor->show();

    /// @todo how to cleanup if destructed?
    Input::global_input().on_event(
        std::bind(&TopWindow::handle_mouse, this, std::placeholders::_1),
    {eventid::raw_pointer_down, eventid::raw_pointer_up, eventid::raw_pointer_move});
}

int TopWindow::handle_mouse(eventid event)
{
    if (m_cursor)
    {
        switch (event)
        {
        case eventid::raw_pointer_down:
        case eventid::raw_pointer_up:
        case eventid::raw_pointer_move:
        {
            auto p = event::pointer().point;

            // don't let the cursor go off the screen
            if (main_screen()->box().contains(Rect(Point(p.x, p.y), m_cursor->size())))
                m_cursor->move(Point(p.x, p.y));

            break;
        }
        default:
            break;
        }
    }

    return 0;
}

}
}
