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

Scrollwheel::Scrollwheel(const Rect& rect, const item_array& items,
                         bool reversed) noexcept
    : StaticGrid(rect, Tuple(1, 3), 1),
      m_items(items),
      m_button_up(make_shared<ImageButton>(Image("@arrow_up.png"))),
      m_button_down(make_shared<ImageButton>(Image("@arrow_down.png"))),
      m_label(make_shared<Label>())
{
    init(reversed);
}

Scrollwheel::Scrollwheel(Frame& parent, const Rect& rect, const item_array& items,
                         bool reversed) noexcept
    : Scrollwheel(rect, items, reversed)
{
    parent.add(*this);
}

Scrollwheel::Scrollwheel(const Rect& rect, int min, int max, int step,
                         bool reversed) noexcept
    : StaticGrid(rect, Tuple(1, 3), 1),
      m_button_up(make_shared<ImageButton>(Image("@arrow_up.png"))),
      m_button_down(make_shared<ImageButton>(Image("@arrow_down.png"))),
      m_label(make_shared<Label>())
{
    for (int i = min; i <= max; i += step)
        m_items.push_back(std::to_string(i));

    init(reversed);
}

void Scrollwheel::init(bool reversed)
{
    set_name("Scrollwheel" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::none);

    if (reversed)
        m_selected = m_items.size() - 1;

    m_label->set_text(m_items[m_selected]);
    add(expand(m_label), 0, 1);

    m_button_up->on_event([this](Event&)
    {
        if (m_selected == m_items.size() - 1)
            m_selected = 0;
        else
            m_selected++;

        m_label->set_text(m_items[m_selected]);

        invoke_handlers(eventid::property_changed);
    }, {eventid::raw_pointer_up});
    add(expand(m_button_up), 0, 0);

    m_button_down->on_event([this](Event&)
    {
        if (m_selected == 0)
            m_selected = m_items.size() - 1;
        else
            m_selected--;

        m_label->set_text(m_items[m_selected]);

        invoke_handlers(eventid::property_changed);
    }, {eventid::raw_pointer_up});
    add(expand(m_button_down), 0, 2);
}

std::string Scrollwheel::get_value() const
{
    return m_items[m_selected];
}

void Scrollwheel::add_item(std::string item)
{
    m_items.push_back(item);
}

unsigned int Scrollwheel::get_index() const
{
    return m_selected;
}

void Scrollwheel::set_index(unsigned int index)
{
    m_selected = index;
    if (m_selected >  m_items.size() - 1)
        m_selected = m_items.size() - 1;

    m_label->set_text(m_items[m_selected]);
    invoke_handlers(eventid::property_changed);
}

}
}
