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

void Frame::add(const std::shared_ptr<Widget>& widget)
{
    if (!widget)
        return;

    widget->set_parent(this);
    m_subordinates.emplace_back(widget);

    if (children().empty())
        children().begin(m_subordinates.begin());

    layout();
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

void Frame::remove_all_basic()
{
    for (auto& i : children())
    {
        // note order here - damage and then unset parent
        i->damage();
        i->m_parent = nullptr;
    }

    m_subordinates.clear();
}

void Frame::remove_all()
{
    remove_all_basic();
    layout();
}

Widget* Frame::hit_test(const DisplayPoint& point)
{
    Point pos = display_to_local(point);

    for (auto& child : detail::reverse_iterate(m_subordinates))
    {
        if (child->box().intersect(pos))
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

    if (local_box().intersect(pos))
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

void Frame::paint_to_file(const std::string& filename)
{
#if CAIRO_HAS_PNG_FUNCTIONS == 1
    /// @todo should this be redirected to parent()?
    std::string name = filename;
    if (name.empty())
        name = fmt::format("{}.png", this->name());

    auto surface = cairo_get_target(screen()->context().get());
    cairo_surface_write_to_png(surface, name.c_str());
#else
    detail::ignoreparam(filename);
    detail::error("png support not available");
#endif
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
