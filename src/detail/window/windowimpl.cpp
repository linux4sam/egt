/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/window/windowimpl.h"
#include "egt/painter.h"
#include "egt/window.h"
#include <spdlog/spdlog.h>

namespace egt
{
inline namespace v1
{

namespace detail
{

WindowImpl::WindowImpl(Window* interface)
    : m_interface(interface)
{
}

void WindowImpl::damage(const Rect& rect)
{
    m_interface->default_damage(rect);
}

Screen* WindowImpl::screen()
{
    assert(m_screen);
    return m_screen;
}

void WindowImpl::resize(const Size& size)
{
    m_interface->default_resize(size);
}

void WindowImpl::set_scale(float scalex, float scaley)
{
    m_interface->default_set_scale(scalex, scaley);
}

void WindowImpl::move(const Point& point)
{
    m_interface->default_move(point);
}

void WindowImpl::top_draw()
{
    m_interface->default_top_draw();
}

void WindowImpl::show()
{
    m_interface->default_show();
}

void WindowImpl::hide()
{
    m_interface->default_hide();
}

void WindowImpl::paint(Painter& painter)
{
    m_interface->default_paint(painter);
}

}
}
}
