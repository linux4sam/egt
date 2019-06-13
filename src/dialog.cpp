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

Dialog::Dialog(const Rect& rect)
    : Popup(rect.size(), rect.point()),
      m_vsizer(std::make_shared<BoxSizer>(orientation::vertical)),
      m_grid(std::make_shared<StaticGrid>(Rect(0, 0, rect.w, (rect.h * 0.15)), Tuple(2, 1), 5)),
      m_button1(std::make_shared<Button>("OK")),
      m_button2(std::make_shared<Button>("Cancel"))
{
    set_name("Dialog" + std::to_string(m_widgetid));

    set_border(theme().default_border());
    set_padding(5);

    m_vsizer->set_align(alignmask::expand);
    add(m_vsizer);

    m_grid->set_align(alignmask::bottom | alignmask::right);

    m_button1->set_align(alignmask::center);
    m_grid->add(expand(m_button1));

    m_button2->set_align(alignmask::center);
    m_grid->add(expand(m_button2));

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
    if (!m_title)
    {
        m_title = std::make_shared<ImageLabel>(Image("@folder.png"), title, Rect(0, 0, w(), (h() * 0.10)));
        m_title->set_align(alignmask::top | alignmask::left);
        m_title->set_text_align(alignmask::left | alignmask::center);
        m_vsizer->add(expand_horizontal(m_title));
    }
    else
    {
        m_title->set_text(title);
    }
}

void Dialog::set_title(const Image& icon, const std::string& title)
{
    if (!m_title)
    {
        m_title = std::make_shared<ImageLabel>(icon, title, Rect(0, 0, w(), (h() * 0.10)));
        m_title->set_align(alignmask::top | alignmask::left);
        m_title->set_text_align(alignmask::left | alignmask::center);
        m_vsizer->add(expand_horizontal(m_title));
    }
    else
    {
        m_title->set_text(title);
    }
}

void Dialog::set_message(const std::string& message)
{
    if (!m_message)
    {
        m_message = std::make_shared<TextBox>(message, Rect(), alignmask::left | alignmask::center);
        m_message->text_flags().set({TextBox::flag::multiline, TextBox::flag::word_wrap});
        m_message->set_color(Palette::ColorId::border, Palette::transparent);
        m_vsizer->add(expand(m_message));
    }
    else
    {
        m_message->set_text(message);
    }
}

void Dialog::set_icon(const Image& icon)
{
    if (m_title)
    {
        m_title->set_image(icon);
    }
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
    if (!m_message)
    {
        m_vsizer->add(widget);
    }
    else
    {
        SPDLOG_DEBUG("Message Box is set");
    }
}

void Dialog::show()
{
    m_vsizer->add(m_grid);
    Popup::show();
}

void Dialog::show_centered()
{
    m_vsizer->add(m_grid);
    Popup::show_centered();
}

}
}
