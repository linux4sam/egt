/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/egtlog.h"
#include "detail/dump.h"
#include "egt/app.h"
#include "egt/detail/layout.h"
#include "egt/detail/math.h"
#include "egt/frame.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/screen.h"
#include <cstdlib>
#include <string>

namespace egt
{
inline namespace v1
{

Frame::Frame(const Rect& rect, const Widget::Flags& flags) noexcept
    : Widget(rect, flags | Widget::Flag::frame)
{
    name("Frame" + std::to_string(m_widgetid));
    m_damage.reserve(10);
}

Frame::Frame(Serializer::Properties& props, bool is_derived) noexcept
    : Widget(props, true)
{
    flags().set(Widget::Flag::frame);

    m_damage.reserve(10);

    if (!is_derived)
        deserialize_leaf(props);
}

Frame::Frame(Frame& parent, const Rect& rect, const Widget::Flags& flags) noexcept
    : Frame(rect, flags)
{
    parent.add(*this);
}

void Frame::add_private(const std::shared_ptr<Widget>& widget, ssize_t pos)
{
    widget->set_parent(this);

    const auto it = (pos >= 0 && static_cast<size_t>(pos) < m_children.size()) ?
                    std::next(m_children.begin(), pos) : m_children.end();

    m_subordinates.emplace(it, widget);
    update_subordinates_ranges();

    layout();
}

void Frame::add(const std::shared_ptr<Widget>& widget)
{
    if (!widget)
        return;

    add_private(widget);
}

void Frame::add_at(const std::shared_ptr<Widget>& widget, size_t pos)
{
    if (!widget)
        return;

    add_private(widget, pos);
}

bool Frame::is_child(Widget* widget) const
{
    if (!widget)
        return false;

    auto i = std::find_if(children().begin(), children().end(),
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });

    return (i != children().end());
}

void Frame::remove(Widget* widget)
{
    if (!widget)
        return;

    auto i = std::find_if(children().begin(), children().end(),
                          [widget](const auto & ptr)
    {
        return ptr.get() == widget;
    });
    if (i != children().end())
    {
        // note order here - damage and then unset parent
        (*i)->damage();
        (*i)->m_parent = nullptr;
        m_subordinates.erase(i);
        if (i == children().begin())
            children().begin(m_subordinates.begin());
        layout();
    }
    else if (widget->m_parent == this)
    {
        widget->m_parent = nullptr;
    }
}

void Frame::remove_at(size_t pos)
{
    if (pos >= count_children())
        return;

    const auto it = std::next(m_children.begin(), pos);
    m_subordinates.erase(it);

    update_subordinates_ranges();

    layout();
}

void Frame::remove_all_basic()
{
    for (auto& i : children())
    {
        // note order here - damage and then unset parent
        i->damage();
        i->m_parent = nullptr;
    }

    m_subordinates.erase(children().begin(), children().end());
    m_components_begin = m_subordinates.begin();
    update_subordinates_ranges();
}

void Frame::remove_all()
{
    remove_all_basic();
    layout();
}

Widget* Frame::hit_test(const DisplayPoint& point)
{
    for (auto& child : detail::reverse_iterate(m_subordinates))
    {
        if (child->hit(point))
        {
            if (child->frame())
            {
                auto frame = dynamic_cast<Frame*>(child.get());
                if (frame)
                    return frame->hit_test(point);
            }
            else
            {
                return child.get();
            }
        }
    }

    if (hit(point))
        return this;

    return nullptr;
}

void Frame::walk(const WalkCallback& callback, int level)
{
    if (!callback(this, level))
        return;

    for (auto& child : children())
        child->walk(callback, level + 1);
}

void Frame::paint_children_to_file()
{
    for (auto& child : children())
    {
        if (child->frame())
        {
            auto frame = std::dynamic_pointer_cast<Frame>(child);
            frame->paint_children_to_file();
        }
        else
        {
            child->paint_to_file();
        }
    }
}

void Frame::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);
    serialize_children(serializer);
}

void Frame::serialize_children(Serializer& serializer) const
{
    for (auto& child : children())
        serializer.add(child.get());
}

void Frame::deserialize_children(const Deserializer& deserializer)
{
    for (auto node = deserializer.first_child("widget");
         node->is_valid();
         node = node->next_sibling("widget"))
        add(node->parse_widget());
}

Frame::~Frame() noexcept
{
    remove_all_basic();
}

void Frame::on_screen_resized()
{
    Widget::on_screen_resized();

    for (auto& child : children())
        child->on_screen_resized();
}

}
}
