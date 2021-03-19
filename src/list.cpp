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
    : Frame(rect),
      m_view(*this, ScrolledView::Policy::never),
      m_sizer(Orientation::vertical, Justification::start)
{
    name("ListBox" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);
    border(theme().default_border());

    m_sizer.align(AlignFlag::expand_horizontal);

    m_view.add(m_sizer);

    auto carea = content_area();
    if (!carea.empty())
    {
        m_view.box(to_child(carea));
        m_sizer.resize(carea.size());
    }

    for (auto& i : items)
        add_item_private(i);
}

ListBox::ListBox(Serializer::Properties& props) noexcept
    : Frame(props),
      m_view(*this, ScrolledView::Policy::never),
      m_sizer(Orientation::vertical, Justification::start)
{
    name("ListBox" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);
    border(theme().default_border());

    m_sizer.align(AlignFlag::expand_horizontal);

    m_view.add(m_sizer);

    auto carea = content_area();
    if (!carea.empty())
    {
        m_view.box(to_child(carea));
        m_sizer.resize(carea.size());
    }

    deserialize(props);
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
    item->align(AlignFlag::expand_horizontal);

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
        Point pos = display_to_local(event.pointer().point);

        for (size_t i = 0; i < m_sizer.count_children(); i++)
        {
            auto cbox = m_sizer.child_at(i)->box();
            cbox.y(cbox.y() + m_view.offset().y());

            if (cbox.intersect(pos))
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

    Frame::handle(event);
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

void ListBox::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);

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
        if (std::get<0>(p) == "item")
        {
            std::string text;
            Image image;
            AlignFlags text_align;
            auto attrs = std::get<2>(p);
            for (const auto& i : attrs)
            {
                if (i.first == "image")
                    image = Image(i.second);

                if (i.first == "text_align")
                    text_align = AlignFlags(i.second);

                if (i.first == "selected" && detail::from_string(i.second))
                    selected(item_count() - 1);
            }

            add_item(std::make_shared<StringItem>(std::get<1>(p), image, Rect(), text_align));
            return true;
        }
        return false;
    }), props.end());
}

}
}
