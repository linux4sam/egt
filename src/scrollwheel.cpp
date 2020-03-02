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

EGT_EMBED(internal_arrow_up, SRCDIR "/icons/32px/arrow_up.png")
EGT_EMBED(internal_arrow_down, SRCDIR "/icons/32px/arrow_down.png")

namespace egt
{
inline namespace v1
{

Scrollwheel::Scrollwheel(const ItemArray& items) noexcept
    : Scrollwheel( {}, items)
{}

Scrollwheel::Scrollwheel(const Rect& rect, const ItemArray& items) noexcept
    : StaticGrid(rect, std::make_tuple(1, 3), 1),
      m_items(items),
      m_button_up(std::make_shared<ImageButton>(Image("res:internal_arrow_up"))),
      m_button_down(std::make_shared<ImageButton>(Image("res:internal_arrow_down"))),
      m_label(std::make_shared<Label>())
{
    init();

    m_button_up->on_click([this](Event&)
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

        m_label->text(m_items[m_selected]);

        on_value_changed.invoke();
    });

    m_button_down->on_click([this](Event&)
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

        m_label->text(m_items[m_selected]);

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
    : StaticGrid(rect, std::make_tuple(1, 3), 1),
      m_button_up(std::make_shared<ImageButton>(Image("res:internal_arrow_up"))),
      m_button_down(std::make_shared<ImageButton>(Image("res:internal_arrow_down"))),
      m_label(std::make_shared<Label>())
{
    for (auto i = min; i <= max; i += step)
        m_items.push_back(std::to_string(i));

    init();

    m_button_up->on_click([this](Event&)
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

        m_label->text(m_items[m_selected]);

        on_value_changed.invoke();
    });

    m_button_down->on_click([this](Event&)
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

        m_label->text(m_items[m_selected]);

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

    m_button_up->fill_flags().clear();
    m_button_down->fill_flags().clear();
    m_label->fill_flags().clear();

    fill_flags().clear();

    m_label->text_align(AlignFlag::center);
    if (!m_items.empty())
        m_label->text(m_items[m_selected]);

    m_label->detach();
    m_button_up->detach();
    m_button_down->detach();

    if (m_orient == Orientation::vertical)
    {
        reallocate(std::make_tuple(1, 3));
        add(expand(m_label), 0, 1);
        add(expand(m_button_up), 0, 0);
        add(expand(m_button_down), 0, 2);
    }
    else
    {
        reallocate(std::make_tuple(3, 1));
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
        m_label->text(m_items[m_selected]);
        on_value_changed.invoke();
    }
}

void Scrollwheel::reversed(bool enabled)
{
    m_reversed = enabled;
}

void Scrollwheel::image(const Image& down_image, const Image& up_image)
{
    m_button_down->image(down_image);
    m_button_up->image(up_image);
}

}
}
