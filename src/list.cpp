/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/imagecache.h"
#include "egt/frame.h"
#include "egt/input.h"
#include "egt/list.h"
#include "egt/painter.h"
#include "egt/string.h"

namespace egt
{
inline namespace v1
{

ListBox::ListBox(const ItemArray& items) noexcept
    : ListBox(items, Rect())
{}

ListBox::ListBox(const Rect& rect) noexcept
    : ListBox(ItemArray(), rect)
{}

ListBox::ListBox(const ItemArray& items, const Rect& rect) noexcept
    : Widget(rect),
      m_view(),
      m_sizer(Orientation::vertical, Justification::start)
{
    name("ListBox" + std::to_string(m_widgetid));

    add_component(m_view);

    fill_flags(Theme::FillFlag::blend);
    border(theme().default_border());

    m_sizer.align(AlignFlag::expand_horizontal);

    m_view.add(m_sizer);

    auto carea = content_area();
    if (!carea.empty())
    {
        m_view.box(to_subordinate(carea));
        m_sizer.resize(carea.size());
    }

    for (auto& i : items)
        add_item_private(i);
}

ListBox::ListBox(Serializer::Properties& props, bool is_derived) noexcept
    : Widget(props, true),
      m_view(),
      m_sizer(Orientation::vertical, Justification::start)
{
    add_component(m_view);

    m_sizer.align(AlignFlag::expand_horizontal);

    m_view.add(m_sizer);

    auto carea = content_area();
    if (!carea.empty())
    {
        m_view.box(to_subordinate(carea));
        m_sizer.resize(carea.size());
    }

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

ListBox::ListBox(Frame& parent, const ItemArray& items, const Rect& rect) noexcept
    : ListBox(items, rect)
{
    parent.add(*this);
}

void ListBox::add_item(const std::shared_ptr<StringItem>& item)
{
    add_item_private(item);
}

void ListBox::add_item_private(const std::shared_ptr<StringItem>& item)
{
    if (m_orient == Orientation::vertical)
        item->align(AlignFlag::expand_horizontal);
    else
        item->align(AlignFlag::expand_vertical);

    m_sizer.add(item);

    // automatically select the first item
    if (m_sizer.count_children() == 1)
    {
        m_sizer.child_at(0)->checked(true);
    }

    on_items_changed.invoke();
}

std::shared_ptr<StringItem> ListBox::item_at(size_t index) const
{
    std::shared_ptr<StringItem> item =
        std::dynamic_pointer_cast<StringItem>(m_sizer.child_at(index));
    return item;
}

void ListBox::remove_item(StringItem* item)
{
    if (m_sizer.is_child(item))
    {
        const auto checked = item->checked();

        m_sizer.remove(item);

        on_items_changed.invoke();

        if (checked)
        {
            if (m_sizer.count_children())
                selected(m_sizer.count_children() - 1);
        }
    }
}

void ListBox::handle(Event& event)
{
    switch (event.id())
    {
    case EventId::pointer_click:
    {
        for (size_t i = 0; i < m_sizer.count_children(); i++)
        {
            auto item = m_sizer.child_at(i);

            if (item->hit(event.pointer().point))
            {
                selected(i);
                break;
            }
        }

        event.stop();
        break;
    }
    case EventId::raw_pointer_down:
    case EventId::raw_pointer_up:
        return;
    default:
        break;
    }

    Widget::handle(event);
}

void ListBox::selected(size_t index)
{
    if (index < m_sizer.count_children())
    {
        bool changed = false;
        for (size_t i = 0; i < m_sizer.count_children(); i++)
        {
            if (m_sizer.child_at(i)->checked() && i != index)
            {
                m_sizer.child_at(i)->checked(false);
                changed = true;
            }
        }

        m_sizer.child_at(index)->checked(true);

        if (changed)
        {
            damage();
            on_selected_changed.invoke();
        }

        on_selected.invoke(index);
    }
}

ssize_t ListBox::selected() const
{
    for (size_t i = 0; i < m_sizer.count_children(); i++)
    {
        if (m_sizer.child_at(i)->checked())
            return i;
    }

    return -1;
}

void ListBox::clear()
{
    if (m_sizer.count_children())
    {
        m_sizer.remove_all();
        on_items_changed.invoke();
    }
}

void ListBox::scroll_top()
{
    m_view.offset(Point(m_view.offset().x(), 0));
}

void ListBox::scroll_bottom()
{
    m_view.offset(Point(m_view.offset().x(), m_view.offset_max().y()));
}

void ListBox::scroll_beginning()
{
    m_view.offset(m_view.offset_min());
}

void ListBox::scroll_end()
{
    m_view.offset(m_view.offset_max());
}

void ListBox::orient(Orientation orient)
{
    // Discard invalid values.
    if (orient != Orientation::vertical && orient != Orientation::horizontal)
        return;

    if (detail::change_if_diff<>(m_orient, orient))
    {
        m_sizer.orient(orient);
        if (orient == Orientation::vertical)
        {
            m_sizer.align(AlignFlag::expand_horizontal);
            for (const auto& child : m_sizer.children())
                child->align(AlignFlag::expand_horizontal);
        }
        else
        {
            m_sizer.align(AlignFlag::expand_vertical);
            for (const auto& child : m_sizer.children())
                child->align(AlignFlag::expand_vertical);
        }
        layout();
    }
}

void ListBox::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);

    serializer.add_property("orient", std::string(detail::enum_to_string(m_orient)));

    for (size_t i = 0; i < m_sizer.count_children(); i++)
    {
        Serializer::Attributes attrs;
        StringItem* item = dynamic_cast<StringItem*>(m_sizer.child_at(i).get());
        if (item)
        {
            if (!item->image().empty())
                attrs.push_back({"image", item->image().uri()});

            if (!item->text().empty())
                attrs.push_back({"text_align", item->text_align().to_string()});

            if (static_cast<ssize_t>(i) == selected())
                attrs.push_back({"selected", "true"});

            serializer.add_property("item", item->text(), attrs);
        }
    }
}

void ListBox::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        if (std::get<0>(p) == "orient")
        {
            orient(detail::enum_from_string<Orientation>(std::get<1>(p)));
            return true;
        }
        else if (std::get<0>(p) == "item")
        {
            std::string text;
            Image image;
            AlignFlags text_align;
            auto attrs = std::get<2>(p);
            bool is_selected = false;
            for (const auto& i : attrs)
            {
                if (i.first == "image")
                    image = Image(i.second);

                if (i.first == "text_align")
                    text_align = AlignFlags(i.second);

                if (i.first == "selected" && detail::from_string(i.second))
                    is_selected = true;
            }

            add_item(std::make_shared<StringItem>(std::get<1>(p), image, Rect(), text_align));
            if (is_selected)
                selected(item_count() - 1);
            return true;
        }
        return false;
    }), props.end());
}

}
}
