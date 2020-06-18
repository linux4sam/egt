/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "detail/dump.h"
#include "detail/window/basicwindow.h"
#include "detail/window/planewindow.h"
#include "egt/app.h"
#include "egt/detail/math.h"
#include "egt/detail/meta.h"
#include "egt/detail/screen/kmsscreen.h"
#include "egt/embed.h"
#include "egt/input.h"
#include "egt/label.h"
#include "egt/painter.h"
#include "egt/window.h"
#include <algorithm>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#ifdef SRCDIR
EGT_EMBED(internal_cursor, SRCDIR "/icons/16px/cursor.png")
#endif

namespace egt
{
inline namespace v1
{

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
    Application::instance().m_windows.push_back(this);
}

Window::Window(Frame& parent,
               const Rect& rect,
               PixelFormat format_hint,
               WindowHint hint)
    : Window(rect, format_hint, hint)
{
    parent.add(*this);
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
    auto reset = detail::on_scope_exit([this]() { m_in_draw = false; });

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
    if (unlikely(Application::instance().m_main_window == this))
        return;

    if (m_impl)
        m_impl->resize(size);
}

void Window::scale(float hscale, float vscale)
{
    // cannot resize if we are screen
    if (unlikely(Application::instance().m_main_window == this))
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

    if (!Application::instance().m_main_window)
    {
        Application::instance().m_main_window = this;
        if (Application::instance().screen())
            m_box = Application::instance().screen()->box();
        m_impl = std::make_unique<detail::BasicTopWindow>(this);
    }
    else
    {
        m_box = rect;

        try
        {
            switch (hint)
            {
            case WindowHint::software:
                m_impl = std::make_unique<detail::BasicWindow>(this);
                break;
            /// NOLINTNEXTLINE(bugprone-branch-clone)
            case WindowHint::overlay:
            case WindowHint::heo_overlay:
            case WindowHint::cursor_overlay:
#ifdef HAVE_LIBPLANES
                m_impl = std::make_unique<detail::PlaneWindow>(this, format_hint, hint);
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
                m_impl = std::make_unique<detail::PlaneWindow>(this, format_hint, hint);
                flags().set(Widget::Flag::plane_window);
            }
            catch (std::exception& e)
            {
                SPDLOG_DEBUG("non-fatal exception: {}", e.what());
#endif

                m_impl = std::make_unique<detail::BasicWindow>(this);
#ifdef HAVE_LIBPLANES
            }
#endif
        }
    }

    assert(m_impl);

    SPDLOG_DEBUG("{} backend is {}", name(),
                 plane_window() ? "PlaneWindow" : "BasicWindow");
}

void Window::main_window()
{
    if (!Application::check_instance())
        throw std::runtime_error("no application instance");

    Application::instance().m_main_window = this;

    if (Application::check_instance() && Application::instance().screen())
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
        m_background = std::make_unique<ImageLabel>(image);
        m_background->align(AlignFlag::expand);
        m_background->image_align(AlignFlag::expand);
        add(*m_background);
        m_background->zorder_bottom();
    }
}

Window::~Window() noexcept
{
    if (Application::check_instance())
    {
        auto i = find(Application::instance().m_windows.begin(),
                      Application::instance().m_windows.end(), this);
        if (i != Application::instance().m_windows.end())
        {
            Application::instance().m_windows.erase(i);
        }

        if (Application::instance().m_main_window == this)
        {
            Application::instance().m_main_window = nullptr;

            for (auto& window : Application::instance().m_windows)
            {
                window->main_window();
                break;
            }
        }

        if (Application::instance().m_modal_window == this)
        {
            Application::instance().m_modal_window = nullptr;
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
        no_layout(true);
        readonly(true);
    }

    EGT_OPS_NOCOPY_MOVE(CursorWindow);
    ~CursorWindow() noexcept override = default;

    void handle(Event&) override
    {
        // no events handled
    }

    std::shared_ptr<ImageLabel> m_label;
};

void TopWindow::hide_cursor()
{
    if (m_cursor)
        m_cursor->hide();
}

void TopWindow::show_cursor(const Image& image)
{
    m_cursor = std::make_unique<CursorWindow>(image);
    if (Application::instance().screen())
        m_cursor->move(Application::instance().screen()->box().center());
    add(*m_cursor);
    m_cursor->show();

    m_handle = Input::global_input().on_event(
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

TopWindow::~TopWindow() noexcept
{
    Input::global_input().remove_handler(m_handle);
}

}
}
