/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "egt/detail/enum.h"
#include "egt/event.h"
#include "egt/input.h"
#include <ostream>
#include <string>

namespace egt
{
inline namespace v1
{

template<>
const std::pair<EventId, char const*> detail::EnumStrings<EventId>::data[] =
{
    {EventId::none, "none"},
    {EventId::raw_pointer_down, "raw_pointer_down"},
    {EventId::raw_pointer_up, "raw_pointer_up"},
    {EventId::raw_pointer_move, "raw_pointer_move"},
    {EventId::pointer_click, "pointer_click"},
    {EventId::pointer_dblclick, "pointer_dblclick"},
    {EventId::pointer_hold, "pointer_hold"},
    {EventId::pointer_drag_start, "pointer_drag_start"},
    {EventId::pointer_drag, "pointer_drag"},
    {EventId::pointer_drag_stop, "pointer_drag_stop"},
    {EventId::keyboard_down, "keyboard_down"},
    {EventId::keyboard_up, "keyboard_up"},
    {EventId::keyboard_repeat, "keyboard_repeat"},
};

std::ostream& operator<<(std::ostream& os, const EventId& event)
{
    return os << detail::enum_to_string(event);
}

void Event::grab(Widget* widget)
{
    stop();
    detail::mouse_grab(widget);
}

template<>
const std::pair<Pointer::Button, char const*> detail::EnumStrings<Pointer::Button>::data[] =
{
    {Pointer::Button::none, "none"},
    {Pointer::Button::left, "left"},
    {Pointer::Button::middle, "middle"},
    {Pointer::Button::right, "right"},
};

std::ostream& operator<<(std::ostream& os, const Pointer::Button& btn)
{
    return os << detail::enum_to_string(btn);
}

std::ostream& operator<<(std::ostream& os, const Pointer& pointer)
{
    return os << fmt::format("point({}) drag_point({}) btn({}) slot({})",
                             pointer.point,
                             pointer.drag_start,
                             pointer.btn,
                             pointer.slot);
}

std::ostream& operator<<(std::ostream& os, const Key& key)
{
    return os << fmt::format("keycode({}) unicode({})",
                             key.keycode,
                             key.unicode);
}

std::ostream& operator<<(std::ostream& os, const Event& event)
{
    os << event.id();
    switch (event.id())
    {
    case EventId::raw_pointer_down:
    case EventId::raw_pointer_up:
    case EventId::raw_pointer_move:
    case EventId::pointer_click:
    case EventId::pointer_dblclick:
    case EventId::pointer_hold:
    case EventId::pointer_drag_start:
    case EventId::pointer_drag:
    case EventId::pointer_drag_stop:
        os << " " << event.pointer();
        break;
    case EventId::keyboard_down:
    case EventId::keyboard_up:
    case EventId::keyboard_repeat:
        os << " " << event.key();
        break;
    default:
        break;
    }
    return os;
}

}
}
