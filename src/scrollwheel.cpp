/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
#include "egt/embed.h"
#include "egt/grid.h"
#include "egt/scrollwheel.h"
#include <algorithm>
#include <memory>
#include <string>

#ifdef SRCDIR
EGT_EMBED(internal_arrow_up, SRCDIR "/icons/32px/arrow_up.png")
EGT_EMBED(internal_arrow_down, SRCDIR "/icons/32px/arrow_down.png")
#endif

namespace egt
{
inline namespace v1
{

Scrollwheel::Scrollwheel(const ItemArray& items) noexcept
    : Scrollwheel( {}, items)
{}

Scrollwheel::Scrollwheel(const Rect& rect, ItemArray items) noexcept
    : StaticGrid(rect, StaticGrid::GridSize(1, 3)),
      m_items(std::move(items)),
      m_button_up(Image("res:internal_arrow_up")),
      m_button_down(Image("res:internal_arrow_down"))
{
    init();

    m_button_up.on_click([this](Event&)
    {
        if (m_items.empty())
            return;

        if (m_reversed)
        {
            if (m_selected == 0)
                m_selected = m_items.size() - 1;
            else
                m_selected--;
        }
        else
        {
            if (m_selected == m_items.size() - 1)
                m_selected = 0;
            else
                m_selected++;
        }

        m_label.text(m_items[m_selected]);

        on_value_changed.invoke();
    });

    m_button_down.on_click([this](Event&)
    {
        if (m_items.empty())
            return;

        if (m_reversed)
        {
            if (m_selected == m_items.size() - 1)
                m_selected = 0;
            else
                m_selected++;
        }
        else
        {
            if (m_selected == 0)
                m_selected = m_items.size() - 1;
            else
                m_selected--;
        }

        m_label.text(m_items[m_selected]);

        on_value_changed.invoke();
    });
}

Scrollwheel::Scrollwheel(Frame& parent, const ItemArray& items) noexcept
    : Scrollwheel( {}, items)
{
    parent.add(*this);
}

Scrollwheel::Scrollwheel(Frame& parent, const Rect& rect, const ItemArray& items) noexcept
    : Scrollwheel(rect, items)
{
    parent.add(*this);
}

Scrollwheel::Scrollwheel(const Rect& rect, int min, int max, int step) noexcept
    : StaticGrid(rect, StaticGrid::GridSize(1, 3)),
      m_button_up(Image("res:internal_arrow_up")),
      m_button_down(Image("res:internal_arrow_down"))
{
    for (auto i = min; i <= max; i += step)
        m_items.push_back(std::to_string(i));

    init();

    m_button_up.on_click([this](Event&)
    {
        if (m_items.empty())
            return;

        if (m_reversed)
        {
            if (m_selected == 0)
                m_selected = m_items.size() - 1;
            else
                m_selected--;
        }
        else
        {
            if (m_selected == m_items.size() - 1)
                m_selected = 0;
            else
                m_selected++;
        }

        m_label.text(m_items[m_selected]);

        on_value_changed.invoke();
    });

    m_button_down.on_click([this](Event&)
    {
        if (m_items.empty())
            return;

        if (m_reversed)
        {
            if (m_selected == m_items.size() - 1)
                m_selected = 0;
            else
                m_selected++;
        }
        else
        {
            if (m_selected == 0)
                m_selected = m_items.size() - 1;
            else
                m_selected--;
        }

        m_label.text(m_items[m_selected]);

        on_value_changed.invoke();
    });
}

void Scrollwheel::orient(Orientation orient)
{
    if (orient != Orientation::horizontal &&
        orient != Orientation::vertical)
    {
        orient = Orientation::vertical;
    }

    if (detail::change_if_diff<>(m_orient, orient))
    {
        auto s = selected();
        init(); // todo
        selected(s);
    }
}

void Scrollwheel::init()
{
    name("Scrollwheel" + std::to_string(m_widgetid));

    horizontal_space(1);
    vertical_space(1);

    m_button_up.fill_flags().clear();
    m_button_down.fill_flags().clear();
    m_label.fill_flags().clear();

    fill_flags().clear();

    m_label.text_align(AlignFlag::center);
    if (!m_items.empty())
        m_label.text(m_items[m_selected]);

    m_label.detach();
    m_button_up.detach();
    m_button_down.detach();

    if (m_orient == Orientation::vertical)
    {
        reallocate(StaticGrid::GridSize(1, 3));
        add(expand(m_label), 0, 1);
        add(expand(m_button_up), 0, 0);
        add(expand(m_button_down), 0, 2);
    }
    else
    {
        reallocate(StaticGrid::GridSize(3, 1));
        add(expand(m_button_up), 0, 0);
        add(expand(m_label), 1, 0);
        add(expand(m_button_down), 2, 0);
    }
}

std::string Scrollwheel::value() const
{
    if (m_items.empty())
        return {};

    return m_items[m_selected];
}

void Scrollwheel::add_item(const std::string& item)
{
    m_items.push_back(item);

    if (m_selected < m_items.size())
        m_label.text(m_items[m_selected]);
}

bool Scrollwheel::remove_item(const std::string& item)
{
    auto it = std::find(m_items.begin(), m_items.end(), item);

    if (it == m_items.end())
        return false;

    m_items.erase(it);

    if (m_selected > m_items.size() - 1)
        m_selected = m_items.size() - 1;

    m_label.text(m_items[m_selected]);

    return true;
}

void Scrollwheel::clear_items()
{
    m_items.clear();
    m_selected = 0;
    m_label.text("");
}

size_t Scrollwheel::selected() const
{
    return m_selected;
}

void Scrollwheel::selected(size_t index)
{
    if (m_items.empty())
        return;

    if (index > m_items.size() - 1)
        index = m_items.size() - 1;

    if (detail::change_if_diff<>(m_selected, index))
    {
        m_label.text(m_items[m_selected]);
        on_value_changed.invoke();
    }
}

void Scrollwheel::reversed(bool enabled)
{
    m_reversed = enabled;
}

void Scrollwheel::image_down(const Image& image)
{
    m_button_down.image(image);
}

void Scrollwheel::image_up(const Image& image)
{
    m_button_up.image(image);
}

void Scrollwheel::serialize(Serializer& serializer) const
{
    Widget::serialize(serializer);

    serializer.add_property("image_up", m_button_up.image().uri());
    serializer.add_property("image_down", m_button_down.image().uri());
    if (reversed())
        serializer.add_property("reversed", reversed());
    serializer.add_property("orient", std::string(detail::enum_to_string(m_orient)));

    size_t index = 0;
    for (const auto& item : m_items)
    {
        if (index == selected())
            serializer.add_property("item", item, {{"selected", "true"}});
        else
            serializer.add_property("item", item);

        ++index;
    }
}

void Scrollwheel::deserialize(const std::string& name, const std::string& value,
                              const Serializer::Attributes& attrs)
{
    switch (detail::hash(name))
    {
    case detail::hash("image_up"):
        image_up(Image(value));
        break;
    case detail::hash("image_down"):
        image_down(Image(value));
        break;
    case detail::hash("reversed"):
        reversed(detail::from_string(value));
        break;
    case detail::hash("orient"):
        orient(detail::enum_from_string<Orientation>(value));
        break;
    case detail::hash("item"):
        add_item(value);
        for (const auto& i : attrs)
        {
            if (i.first == "selected" && detail::from_string(i.second))
                selected(item_count() - 1);
        }
        break;
    default:
        Widget::deserialize(name, value, attrs);
        break;
    }
}

}
}
