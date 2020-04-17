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
      m_button1("OK"),
      m_button2("Cancel")
{
    name("Dialog" + std::to_string(m_widgetid));

    border(theme().default_border());
    padding(5);

    m_layout.align(AlignFlag::expand);
    add(m_layout);

    m_title.text_align(AlignFlag::left | AlignFlag::center);
    m_layout.add(expand_horizontal(m_title));

    m_layout.add(expand(m_content));

    auto grid = std::make_shared<StaticGrid>(Rect(0, 0, 0, (rect.height() * 0.15)), StaticGrid::GridSize(2, 1), 5);
    grid->align(AlignFlag::bottom | AlignFlag::expand_horizontal);
    m_layout.add(grid);

    m_button1.align(AlignFlag::center);
    grid->add(expand(m_button1));

    m_button2.align(AlignFlag::center);
    grid->add(expand(m_button2));

    m_button1.on_event([this](Event & event)
    {
        event.stop();
        on_button1_click.invoke();
        hide();
    }, {EventId::pointer_click});

    m_button2.on_event([this](Event & event)
    {
        event.stop();
        on_button2_click.invoke();
        hide();
    }, {EventId::pointer_click});

}

void Dialog::title(const std::string& title)
{
    m_title.text(title);
}

void Dialog::title(const Image& icon, const std::string& title)
{
    m_title.image(icon);
    m_title.text(title);
}

void Dialog::icon(const Image& icon)
{
    m_title.image(icon);
}

void Dialog::button(ButtonId button, const std::string& text)
{
    if (button == ButtonId::button1)
    {
        if (text.empty())
            m_button1.hide();
        else
        {
            m_button1.text(text);
            m_button1.show();
        }
    }
    else if (button == ButtonId::button2)
    {
        if (text.empty())
            m_button2.hide();
        else
        {
            m_button2.text(text);
            m_button2.show();
        }
    }
}

void Dialog::widget(const std::shared_ptr<Widget>& widget)
{
    m_content.add(widget);
}

}
}
