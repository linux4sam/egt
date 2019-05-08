/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SIDEBOARD_H
#define EGT_SIDEBOARD_H

/**
 * @file
 * @brief Sideboard widget.
 */

#include <egt/animation.h>
#include <egt/easing.h>
#include <egt/screen.h>
#include <egt/window.h>

namespace egt
{
inline namespace v1
{

/**
 * SideBoard widget for a sliding board that slides on and off the screen.
 *
 * This is a widget that manages a Window that slides on and off the screen,
 * with only a small portion of it shown so that sliding it out can be
 * initiated by default.
 */
class SideBoard : public Window
{
public:

    constexpr static const auto HANDLE_WIDTH = 50;

    enum class flags
    {
        left = 1 << 0,
        right = 1 << 1,
        top = 1 << 2,
        bottom = 1 << 3,
    };

    explicit SideBoard(flags f = flags::left,
                       std::chrono::milliseconds open_duration = std::chrono::milliseconds(1000),
                       easing_func_t open_func = easing_cubic_easeinout,
                       std::chrono::milliseconds close_duration = std::chrono::milliseconds(1000),
                       easing_func_t close_func = easing_circular_easeinout)
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

    virtual int handle(eventid event) override
    {
        auto ret = Window::handle(event);
        if (ret)
            return ret;

        switch (event)
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

        return ret;
    }

    virtual void close()
    {
        if (m_oanim.running())
        {
            m_oanim.stop();
            m_canim.starting(m_oanim.current());
        }
        m_canim.start();
        m_dir = false;
    }

    virtual void open()
    {
        if (m_oanim.running())
        {
            m_canim.stop();
            m_oanim.starting(m_canim.current());
        }
        m_oanim.start();
        m_dir = true;
    }

    virtual ~SideBoard() = default;

protected:

    inline bool is_set(flags f) const;

    /**
     * Sideboard flags.
     */
    flags m_side_flags{flags::left};

    /**
     * Open animation.
     */
    PropertyAnimator m_oanim;

    /**
     * Close animation.
     */
    PropertyAnimator m_canim;

    /**
     * State of the current direction.
     */
    bool m_dir{false};
};

ENABLE_BITMASK_OPERATORS(SideBoard::flags)

bool SideBoard::is_set(flags f) const
{
    return ((m_side_flags & f) == f);
}

}
}

#endif
