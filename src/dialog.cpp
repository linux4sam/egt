/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/dialog.h"
#include <memory>

namespace egt
{
inline namespace v1
{

Dialog::Dialog(const Rect& rect) noexcept
    : Popup(rect.size(), rect.point()),
      m_grid(std::make_shared<egt::StaticGrid>(Size(0, (rect.height() * 0.15)), StaticGrid::GridSize(2, 1))),
      m_button1("OK"),
      m_button2("Cancel")
{
    name("Dialog" + std::to_string(m_widgetid));
    initialize();
}

Dialog::Dialog(Serializer::Properties& props, bool is_derived) noexcept
    : Popup(props, true),
      m_grid(std::make_shared<egt::StaticGrid>(Size(0, (box().height() * 0.15)), StaticGrid::GridSize(2, 1))),
      m_button1("OK"),
      m_button2("Cancel")
{
    initialize(false);

    if (!is_derived)
        deserialize_leaf(props);
}

void Dialog::initialize(bool init_inherited_properties)
{
    m_grid->margin(5);
    m_grid->horizontal_space(5);
    m_grid->vertical_space(5);

    if (init_inherited_properties)
    {
        border(theme().default_border());
        padding(5);
    }

    m_layout.align(AlignFlag::expand);
    add(m_layout);

    m_title.text_align(AlignFlag::left | AlignFlag::center_vertical);
    m_layout.add(expand_horizontal(m_title));

    m_layout.add(expand(m_content));

    m_grid->align(AlignFlag::bottom | AlignFlag::expand_horizontal);
    m_layout.add(m_grid);

    m_button1.align(AlignFlag::center);
    m_grid->add(expand(m_button1));

    m_button2.align(AlignFlag::center);
    m_grid->add(expand(m_button2));

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

Size Dialog::min_size_hint() const
{
    auto min_height = std::max(m_button1.min_size_hint().height(),
                               m_button2.min_size_hint().height()) / 0.15;

    if (min_height > Application::instance().screen()->size().height())
        min_height = Application::instance().screen()->size().height();

    auto min_width = std::max(m_button1.min_size_hint().width(),
                              m_button2.min_size_hint().width()) * 2;

    if (min_width > Application::instance().screen()->size().width())
        min_width = Application::instance().screen()->size().width();

    return Size(min_width, min_height);
}

void Dialog::resize(const Size& size)
{
    Popup::resize(size);
    m_grid->resize(Size(size.width(), size.height() * 0.15));
}


void Dialog::layout()
{
    // Resize to min_size_hint if needed and allowed.
    Widget::layout();
    Popup::layout();
}

}
}
