/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/window/basicwindow.h"
#include "detail/window/planewindow.h"
#include "egt/app.h"
#include "egt/detail/math.h"
#include "egt/detail/meta.h"
#include "egt/detail/screen/kmsscreen.h"
#include "egt/input.h"
#include "egt/label.h"
#include "egt/painter.h"
#include "egt/window.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace egt
{
inline namespace v1
{

static std::vector<Window*> the_windows;
static Window* the_main_window = nullptr;
static Window* the_modal_window = nullptr;

Window* main_window()
{
    return the_main_window;
}

Window* modal_window()
{
    return the_modal_window;
}

namespace detail
{
void set_modal_window(Window* window)
{
    the_modal_window = window;
}
}

std::vector<Window*>& windows()
{
    return the_windows;
}

const pixel_format Window::DEFAULT_FORMAT = pixel_format::argb8888;

Window::Window(const Rect& rect,
               pixel_format format,
               windowhint hint)
// by default, windows are hidden
    : Frame(rect, {Widget::flag::window, Widget::flag::invisible})
{
    set_name("Window" + std::to_string(m_widgetid));

    // windows are not transparent by default
    set_boxtype(Theme::boxtype::solid | Theme::boxtype::fill);

    // create the window implementation
    create_impl(box(), format, hint);

    // save off the new window to the window list
    windows().push_back(this);
}

void Window::damage(const Rect& rect)
{
    if (m_impl)
        m_impl->damage(rect);
}

Screen* Window::screen() const
{
    if (m_impl)
        return m_impl->screen();
    return nullptr;
}

bool Window::has_screen() const
{
    if (m_impl)
        return m_impl->has_screen();
    return false;
}

void Window::move(const Point& point)
{
    if (point != box().point())
    {
        if (m_impl)
            m_impl->move(point);

        parent_layout();
    }
}

void Window::show()
{
    if (m_impl)
        m_impl->show();
}

void Window::hide()
{
    if (m_impl)
        m_impl->hide();
}

void Window::paint(Painter& painter)
{
    if (m_impl)
        m_impl->paint(painter);
}

void Window::begin_draw()
{
    SPDLOG_TRACE("{} top draw", name());

    if (m_impl)
        m_impl->begin_draw();
}

void Window::allocate_screen()
{
    if (m_impl)
        m_impl->allocate_screen();
}

static inline bool time_child_draw_enabled()
{
    static int value = 0;
    if (value == 0)
    {
        if (std::getenv("EGT_TIME_DRAW"))
            value += 1;
        else
            value -= 1;
    }
    return value == 1;
}

void Window::do_draw()
{
    if (m_damage.empty())
        return;

    // bookkeeping to make sure we don't damage() in draw()
    m_in_draw = true;
    detail::scope_exit reset([this]() { m_in_draw = false; });

    SPDLOG_TRACE("{} do draw", name());

    detail::code_timer(time_child_draw_enabled(), name() + " draw: ", [this]()
    {
        Painter painter(screen()->context());

        for (auto& damage : m_damage)
            draw(painter, damage);

        screen()->flip(m_damage);
        m_damage.clear();
    });
}

void Window::resize(const Size& size)
{
    // cannot resize if we are screen
    if (unlikely(the_main_window == this))
        return;

    if (m_impl)
        m_impl->resize(size);
}

void Window::set_scale(float scalex, float scaley)
{
    // cannot resize if we are screen
    if (unlikely(the_main_window == this))
        return;

    if (m_impl)
        m_impl->set_scale(scalex, scaley);
}

void Window::create_impl(const Rect& rect,
                         pixel_format format,
                         windowhint hint)
{
    detail::ignoreparam(format);
    detail::ignoreparam(hint);

    assert(Application::instance().screen());

    if (!the_main_window)
    {
        the_main_window = this;
        m_box = Application::instance().screen()->box();
        m_impl = detail::make_unique<detail::BasicTopWindow>(this);
    }
    else
    {
        m_box = rect;

        try
        {
            switch (hint)
            {
            case windowhint::software:
                m_impl = detail::make_unique<detail::BasicWindow>(this);
                break;
            case windowhint::overlay:
            case windowhint::heo_overlay:
            case windowhint::cursor_overlay:
#ifdef HAVE_LIBPLANES
                m_impl = detail::make_unique<detail::PlaneWindow>(this, format, hint);
                flags().set(Widget::flag::plane_window);
#endif
                break;
            default:
                break;
            }
        }
        catch (std::exception& e)
        {
            SPDLOG_DEBUG("non-fatal exception: {}", e.what());
        }

        if (!m_impl)
        {
#ifdef HAVE_LIBPLANES
            try
            {
                m_impl = detail::make_unique<detail::PlaneWindow>(this, format, hint);
                flags().set(Widget::flag::plane_window);
            }
            catch (std::exception& e)
            {
                SPDLOG_DEBUG("non-fatal exception: {}", e.what());
#endif

                m_impl = detail::make_unique<detail::BasicWindow>(this);
#ifdef HAVE_LIBPLANES
            }
#endif
        }
    }

    assert(m_impl);

    if (flags().is_set(Widget::flag::plane_window))
    {
        SPDLOG_DEBUG("{} backend is PlaneWindow", name());
    }
    else
    {
        SPDLOG_DEBUG("{} backend is BasicWindow", name());
    }
}

void Window::set_main_window()
{
    the_main_window = this;

    assert(Application::instance().screen());

    m_box = Application::instance().screen()->box();

    damage();
}

void Window::set_background(const Image& image)
{
    set_boxtype(Theme::boxtype::none);

    if (m_background)
    {
        m_background->detach();
        m_background.reset();
    }

    if (!image.empty())
    {
        m_background = std::make_shared<ImageLabel>(image);
        m_background->set_align(alignmask::expand);
        m_background->set_image_align(alignmask::expand);
        add(m_background);
        m_background->zorder_bottom();
    }
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
        : Window(image.size(), pixel_format::argb8888, windowhint::cursor_overlay),
          m_label(new ImageLabel(image))
    {
        set_color(Palette::ColorId::bg, Palette::transparent);
        set_boxtype(Theme::boxtype::fill);
        m_label->set_boxtype(Theme::boxtype::none);
        add(m_label);
        flags().set(Widget::flag::no_layout);
        set_readonly(true);
    }

    virtual void handle(Event&) override
    {
        // no events handled
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
    m_cursor = std::make_shared<CursorWindow>(image);
    m_cursor->move(Application::instance().screen()->box().center());
    add(m_cursor);
    m_cursor->show();

    /// @todo how to cleanup if destructed?
    Input::global_input().on_event(
        std::bind(&TopWindow::handle_mouse, this, std::placeholders::_1),
    {eventid::raw_pointer_down, eventid::raw_pointer_up, eventid::raw_pointer_move});
}

void TopWindow::handle_mouse(Event& event)
{
    if (m_cursor)
    {
        switch (event.id())
        {
        case eventid::raw_pointer_down:
        case eventid::raw_pointer_up:
        case eventid::raw_pointer_move:
        {
            auto p = event.pointer().point;
            m_cursor->move(Point(p.x(), p.y()));

            break;
        }
        default:
            break;
        }
    }
}

}
}
