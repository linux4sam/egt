/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
#include "egt/grid.h"
#include "egt/notebook.h"
#include "egt/scrollwheel.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace egt
{
inline namespace v1
{

Scrollwheel::Scrollwheel(const item_array& items,
                         bool reversed) noexcept
    : Scrollwheel( {}, items, reversed)
{}

Scrollwheel::Scrollwheel(const Rect& rect, const item_array& items,
                         bool reversed) noexcept
    : StaticGrid(rect, std::make_tuple(1, 3), 1),
      m_items(items),
      m_button_up(make_shared<ImageButton>(Image("@arrow_up.png"))),
      m_button_down(make_shared<ImageButton>(Image("@arrow_down.png"))),
      m_label(make_shared<Label>())
{
    init(reversed);

    m_button_up->on_click([this](Event&)
    {
        if (m_items.empty())
            return;

        if (m_selected == m_items.size() - 1)
            m_selected = 0;
        else
            m_selected++;

        m_label->text(m_items[m_selected]);

        invoke_handlers(eventid::property_changed);
    });


    m_button_down->on_click([this](Event&)
    {
        if (m_items.empty())
            return;

        if (m_selected == 0)
            m_selected = m_items.size() - 1;
        else
            m_selected--;

        m_label->text(m_items[m_selected]);

        invoke_handlers(eventid::property_changed);
    });
}

Scrollwheel::Scrollwheel(Frame& parent, const item_array& items,
                         bool reversed) noexcept
    : Scrollwheel( {}, items, reversed)
{
    parent.add(*this);
}

Scrollwheel::Scrollwheel(Frame& parent, const Rect& rect, const item_array& items,
                         bool reversed) noexcept
    : Scrollwheel(rect, items, reversed)
{
    parent.add(*this);
}

Scrollwheel::Scrollwheel(const Rect& rect, int min, int max, int step,
                         bool reversed) noexcept
    : StaticGrid(rect, std::make_tuple(1, 3), 1),
      m_button_up(make_shared<ImageButton>(Image("@arrow_up.png"))),
      m_button_down(make_shared<ImageButton>(Image("@arrow_down.png"))),
      m_label(make_shared<Label>())
{
    for (auto i = min; i <= max; i += step)
        m_items.push_back(std::to_string(i));

    init(reversed);

    m_button_up->on_click([this](Event&)
    {
        if (m_items.empty())
            return;

        if (m_selected == m_items.size() - 1)
            m_selected = 0;
        else
            m_selected++;

        m_label->text(m_items[m_selected]);

        invoke_handlers(eventid::property_changed);
    });


    m_button_down->on_click([this](Event&)
    {
        if (m_items.empty())
            return;

        if (m_selected == 0)
            m_selected = m_items.size() - 1;
        else
            m_selected--;

        m_label->text(m_items[m_selected]);

        invoke_handlers(eventid::property_changed);
    });

}

void Scrollwheel::orient(orientation orient)
{
    if (orient != orientation::horizontal &&
        orient != orientation::vertical)
    {
        orient = orientation::vertical;
    }

    if (detail::change_if_diff<>(m_orient, orient))
    {
        auto s = selected();
        init(false); // todo
        selected(s);
    }
}

void Scrollwheel::init(bool reversed)
{
    name("Scrollwheel" + std::to_string(m_widgetid));

    m_button_up->boxtype(Theme::boxtype::none);
    m_button_down->boxtype(Theme::boxtype::none);
    m_label->boxtype(Theme::boxtype::none);

    boxtype(Theme::boxtype::none);

    if (!m_items.empty() && reversed)
        m_selected = m_items.size() - 1;

    m_label->text_align(alignmask::center);
    if (!m_items.empty())
        m_label->text(m_items[m_selected]);

    m_label->detach();
    m_button_up->detach();
    m_button_down->detach();

    if (m_orient == orientation::vertical)
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
        invoke_handlers(eventid::property_changed);
    }
}

void Scrollwheel::image(const Image& down_image, const Image& up_image)
{
    m_button_down->image(down_image);
    m_button_up->image(up_image);
}

}
}
