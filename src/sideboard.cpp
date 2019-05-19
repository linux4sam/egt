/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/sideboard.h"

namespace egt
{
inline namespace v1
{

SideBoard::SideBoard(flags f,
                     std::chrono::milliseconds open_duration,
                     easing_func_t open_func,
                     std::chrono::milliseconds close_duration,
                     easing_func_t close_func)
    : Window(main_screen()->size() + Size(f == flags::left || f == flags::right ? HANDLE_WIDTH : 0,
                                          f == flags::top || f == flags::bottom ? HANDLE_WIDTH : 0),
             Widget::flags_type(), pixel_format::rgb565),
      m_side_flags(f)
{
    m_oanim.duration(open_duration);
    m_canim.duration(close_duration);
    m_oanim.set_easing_func(open_func);
    m_canim.set_easing_func(close_func);

    if (is_set(flags::left))
    {
        m_oanim.starting(-main_screen()->size().w);
        m_oanim.ending(0);
        m_canim.starting(m_oanim.ending());
        m_canim.ending(m_oanim.starting());

        m_oanim.on_change(std::bind(&SideBoard::set_x, this, std::placeholders::_1));
        m_canim.on_change(std::bind(&SideBoard::set_x, this, std::placeholders::_1));

        move(Point(m_oanim.starting(), 0));
    }
    else if (is_set(flags::right))
    {
        m_oanim.starting(main_screen()->size().w - HANDLE_WIDTH);
        m_oanim.ending(-HANDLE_WIDTH);
        m_canim.starting(m_oanim.ending());
        m_canim.ending(m_oanim.starting());

        m_oanim.on_change(std::bind(&SideBoard::set_x, this, std::placeholders::_1));
        m_canim.on_change(std::bind(&SideBoard::set_x, this, std::placeholders::_1));

        move(Point(m_oanim.starting(), 0));
    }
    else if (is_set(flags::top))
    {
        m_oanim.starting(-main_screen()->size().h);
        m_oanim.ending(0);
        m_canim.starting(m_oanim.ending());
        m_canim.ending(m_oanim.starting());

        m_oanim.on_change(std::bind(&SideBoard::set_y, this, std::placeholders::_1));
        m_canim.on_change(std::bind(&SideBoard::set_y, this, std::placeholders::_1));

        move(Point(0, m_oanim.starting()));
    }
    else if (is_set(flags::bottom))
    {
        m_oanim.starting(main_screen()->size().h - HANDLE_WIDTH);
        m_oanim.ending(-HANDLE_WIDTH);
        m_canim.starting(m_oanim.ending());
        m_canim.ending(m_oanim.starting());

        m_oanim.on_change(std::bind(&SideBoard::set_y, this, std::placeholders::_1));
        m_canim.on_change(std::bind(&SideBoard::set_y, this, std::placeholders::_1));

        move(Point(0, m_oanim.starting()));
    }
}

void SideBoard::handle(Event& event)
{
    Window::handle(event);

    switch (event.id())
    {
    case eventid::pointer_click:
    {
        if (m_dir)
            close();
        else
            open();

        break;
    }
    default:
        break;
    }
}

void SideBoard::close()
{
    if (m_oanim.running())
    {
        m_oanim.stop();
        m_canim.starting(m_oanim.current());
    }
    m_canim.start();
    m_dir = false;
}

void SideBoard::open()
{
    if (m_oanim.running())
    {
        m_canim.stop();
        m_oanim.starting(m_canim.current());
    }
    m_oanim.start();
    m_dir = true;
}


}
}
