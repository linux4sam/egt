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
void modal_window(Window* window)
{
    the_modal_window = window;
}
}

std::vector<Window*>& windows()
{
    return the_windows;
}

const PixelFormat Window::DEFAULT_FORMAT = PixelFormat::argb8888;

Window::Window(const Rect& rect,
               PixelFormat format_hint,
               WindowHint hint)
// by default, windows are hidden
    : Frame(rect, {Widget::Flag::window, Widget::Flag::invisible})
{
    name("Window" + std::to_string(m_widgetid));

    // windows are not transparent by default
    fill_flags(Theme::FillFlag::solid);

    // create the window implementation
    create_impl(box(), format_hint, hint);

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
    detail::ScopeExit reset([this]() { m_in_draw = false; });

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

void Window::scale(float hscale, float vscale)
{
    // cannot resize if we are screen
    if (unlikely(the_main_window == this))
        return;

    if (m_impl)
        m_impl->scale(hscale, vscale);
}

void Window::create_impl(const Rect& rect,
                         PixelFormat format_hint,
                         WindowHint hint)
{
    detail::ignoreparam(format_hint);
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
            case WindowHint::software:
                m_impl = detail::make_unique<detail::BasicWindow>(this);
                break;
            case WindowHint::overlay:
            case WindowHint::heo_overlay:
            case WindowHint::cursor_overlay:
#ifdef HAVE_LIBPLANES
                m_impl = detail::make_unique<detail::PlaneWindow>(this, format_hint, hint);
                flags().set(Widget::Flag::plane_window);
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
                m_impl = detail::make_unique<detail::PlaneWindow>(this, format_hint, hint);
                flags().set(Widget::Flag::plane_window);
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

    if (flags().is_set(Widget::Flag::plane_window))
    {
        SPDLOG_DEBUG("{} backend is PlaneWindow", name());
    }
    else
    {
        SPDLOG_DEBUG("{} backend is BasicWindow", name());
    }
}

void Window::main_window()
{
    the_main_window = this;

    if (Application::instance().screen())
        m_box = Application::instance().screen()->box();

    damage();
}

void Window::background(const Image& image)
{
    fill_flags().clear();

    if (m_background)
    {
        m_background->detach();
        m_background.reset();
    }

    if (!image.empty())
    {
        m_background = std::make_shared<ImageLabel>(image);
        m_background->align(AlignFlag::expand);
        m_background->image_align(AlignFlag::expand);
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
            window->main_window();
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
        : Window(image.size(), PixelFormat::argb8888, WindowHint::cursor_overlay),
          m_label(new ImageLabel(image))
    {
        color(Palette::ColorId::bg, Palette::transparent);
        fill_flags(Theme::FillFlag::blend);
        m_label->fill_flags().clear();
        add(m_label);
        flags().set(Widget::Flag::no_layout);
        readonly(true);
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
    {EventId::raw_pointer_down, EventId::raw_pointer_up, EventId::raw_pointer_move});
}

void TopWindow::handle_mouse(Event& event)
{
    if (m_cursor)
    {
        switch (event.id())
        {
        case EventId::raw_pointer_down:
        case EventId::raw_pointer_up:
        case EventId::raw_pointer_move:
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
