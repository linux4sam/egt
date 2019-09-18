/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/dialog.h"
#include <memory>
#include <spdlog/spdlog.h>

namespace egt
{
inline namespace v1
{

Dialog::Dialog(const Rect& rect) noexcept
    : Popup(rect.size(), rect.point()),
      m_layout(std::make_shared<VerticalBoxSizer>()),
      m_content(std::make_shared<VerticalBoxSizer>()),
      m_title(std::make_shared<ImageLabel>()),
      m_button1(std::make_shared<Button>("OK")),
      m_button2(std::make_shared<Button>("Cancel"))
{
    set_name("Dialog" + std::to_string(m_widgetid));

    set_border(theme().default_border());
    set_padding(5);

    m_layout->set_align(alignmask::expand);
    add(m_layout);

    m_title->set_text_align(alignmask::left | alignmask::center);
    m_layout->add(expand_horizontal(m_title));

    m_layout->add(expand(m_content));

    auto grid = std::make_shared<StaticGrid>(Rect(0, 0, 0, (rect.height() * 0.15)), Tuple(2, 1), 5);
    grid->set_align(alignmask::bottom | alignmask::expand_horizontal);
    m_layout->add(grid);

    m_button1->set_align(alignmask::center);
    grid->add(expand(m_button1));

    m_button2->set_align(alignmask::center);
    grid->add(expand(m_button2));

    m_button1->on_event([this](Event & event)
    {
        event.stop();
        Event event2(eventid::event1);
        invoke_handlers(event2);
        hide();
    }, {eventid::pointer_click});

    m_button2->on_event([this](Event & event)
    {
        event.stop();
        Event event2(eventid::event2);
        invoke_handlers(event2);
        hide();
    }, {eventid::pointer_click});

}

void Dialog::set_title(const std::string& title)
{
    m_title->set_text(title);
}

void Dialog::set_title(const Image& icon, const std::string& title)
{
    m_title->set_image(icon);
    m_title->set_text(title);
}

void Dialog::set_icon(const Image& icon)
{
    m_title->set_image(icon);
}

void Dialog::set_button(buttonid button, const std::string& text)
{
    if (button == buttonid::button1)
    {
        m_button1->set_text(text);
    }
    else if (button == buttonid::button2)
    {
        m_button2->set_text(text);
    }
}

void Dialog::set_widget(const std::shared_ptr<Widget>& widget)
{
    m_content->add(widget);
}

}
}
