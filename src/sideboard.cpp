/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/app.h"
#include "egt/detail/enum.h"
#include "egt/detail/serialize.h"
#include "egt/sideboard.h"
#include <iostream>

namespace egt
{
inline namespace v1
{

static inline Size calculate_size(SideBoard::PositionFlag position)
{
    return Application::instance().screen()->size() +
           Size(position == SideBoard::PositionFlag::left || position == SideBoard::PositionFlag::right ? SideBoard::HANDLE_WIDTH : 0,
                position == SideBoard::PositionFlag::top || position == SideBoard::PositionFlag::bottom ? SideBoard::HANDLE_WIDTH : 0);
}

SideBoard::SideBoard(PositionFlag position,
                     std::chrono::milliseconds open_duration,
                     EasingFunc open_func,
                     std::chrono::milliseconds close_duration,
                     EasingFunc close_func)
    : Window(calculate_size(position), PixelFormat::rgb565),
      m_position(position)
{
    m_oanim.duration(open_duration);
    m_canim.duration(close_duration);
    m_oanim.easing_func(std::move(open_func));
    m_canim.easing_func(std::move(close_func));

    reset_animations();

    switch (m_position)
    {
    case PositionFlag::left:
    {
        m_oanim.on_change([this](PropertyAnimator::Value value) { x(value); });
        m_canim.on_change([this](PropertyAnimator::Value value) { x(value); });

        move(Point(m_oanim.starting(), 0));
        break;
    }
    case PositionFlag::right:
    {
        m_oanim.on_change([this](PropertyAnimator::Value value) { x(value); });
        m_canim.on_change([this](PropertyAnimator::Value value) { x(value); });

        move(Point(m_oanim.starting(), 0));
        break;
    }
    case PositionFlag::top:
    {
        m_oanim.on_change([this](PropertyAnimator::Value value) { y(value); });
        m_canim.on_change([this](PropertyAnimator::Value value) { y(value); });

        move(Point(0, m_oanim.starting()));
        break;
    }
    case PositionFlag::bottom:
    {
        m_oanim.on_change([this](PropertyAnimator::Value value) { y(value); });
        m_canim.on_change([this](PropertyAnimator::Value value) { y(value); });

        move(Point(0, m_oanim.starting()));
        break;
    }
    }
}

void SideBoard::position(PositionFlag position)
{
    if (detail::change_if_diff<>(m_position, position))
    {
        m_oanim.stop();
        m_canim.stop();
        resize(calculate_size(position));
        m_dir = false;
        reset_animations();
        move(Point(m_oanim.starting(), 0));
    }
}

void SideBoard::reset_animations()
{
    switch (m_position)
    {
    case PositionFlag::left:
    {
        m_oanim.starting(-Application::instance().screen()->size().width());
        m_oanim.ending(0);
        m_canim.starting(m_oanim.ending());
        m_canim.ending(m_oanim.starting());
        break;
    }
    case PositionFlag::right:
    {
        m_oanim.starting(Application::instance().screen()->size().width() - HANDLE_WIDTH);
        m_oanim.ending(-HANDLE_WIDTH);
        m_canim.starting(m_oanim.ending());
        m_canim.ending(m_oanim.starting());
        break;
    }
    case PositionFlag::top:
    {
        m_oanim.starting(-Application::instance().screen()->size().height());
        m_oanim.ending(0);
        m_canim.starting(m_oanim.ending());
        m_canim.ending(m_oanim.starting());
        break;
    }
    case PositionFlag::bottom:
    {
        m_oanim.starting(Application::instance().screen()->size().height() - HANDLE_WIDTH);
        m_oanim.ending(-HANDLE_WIDTH);
        m_canim.starting(m_oanim.ending());
        m_canim.ending(m_oanim.starting());
        break;
    }
    }
}

void SideBoard::handle(Event& event)
{
    Window::handle(event);

    switch (event.id())
    {
    case EventId::pointer_click:
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
    m_canim.stop();
    auto running = m_oanim.running();
    m_oanim.stop();
    auto current = m_oanim.current();
    reset_animations();
    if (running)
        m_canim.starting(current);
    m_canim.start();
    m_dir = false;
}

void SideBoard::open()
{
    m_oanim.stop();
    auto running = m_canim.running();
    m_canim.stop();
    auto current = m_canim.current();
    reset_animations();
    if (running)
        m_oanim.starting(current);
    m_oanim.start();
    m_dir = true;
}

void SideBoard::serialize(detail::Serializer& serializer) const
{
    Window::serialize(serializer);

    serializer.add_property("position", detail::enum_to_string(position()));
}

void SideBoard::deserialize(const std::string& name, const std::string& value,
                            const std::map<std::string, std::string>& attrs)
{
    if (name == "position")
        position(detail::enum_from_string<PositionFlag>(value));
    else
        Window::deserialize(name, value, attrs);
}

template<>
std::map<SideBoard::PositionFlag, char const*> detail::EnumStrings<SideBoard::PositionFlag>::data =
{
    {SideBoard::PositionFlag::left, "left"},
    {SideBoard::PositionFlag::right, "right"},
    {SideBoard::PositionFlag::top, "top"},
    {SideBoard::PositionFlag::bottom, "bottom"},
};

std::ostream& operator<<(std::ostream& os, const SideBoard::PositionFlag& flag)
{
    return os << detail::enum_to_string(flag);
}

}
}
