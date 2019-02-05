/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/combo.h"
#include "egt/painter.h"
#include "egt/frame.h"
#include "egt/image.h"
#include "egt/imagecache.h"
#include "egt/window.h"
#include <iostream>

using namespace std;

namespace egt
{
inline namespace v1
{

auto POPUP_OFFSET = 5;

namespace detail
{

ComboBoxPopup::ComboBoxPopup(ComboBox& parent)
    : ComboBoxPopupBase(Size(parent.size().w, ListBox::item_height())),
      m_parent(parent)
{
    for (auto& item : parent.m_items)
        m_list.add_item(new StringItem(item));

    m_list.set_align(alignmask::EXPAND);

    add(&m_list);

    m_list.on_event([this, &parent](eventid event)
    {
        ignoreparam(event);
        parent.set_select(m_list.selected());
        hide();

        return 1;
    }, {eventid::PROPERTY_CHANGED});
}

void ComboBoxPopup::smart_pos()
{
    move(m_parent.box().point() + Point(0, m_parent.size().h + POPUP_OFFSET));

    if (main_screen())
    {
        auto ss = main_screen()->size();
        auto origin = to_screen(box().point());
        auto height =
            std::min(static_cast<default_dim_type>((ss.h - origin.y) / ListBox::item_height()),
                     static_cast<default_dim_type>(m_list.count())) * ListBox::item_height();

        // hack because list child area is smaller by this much
        height += Theme::DEFAULT_BORDER_WIDTH * 3;

        resize(Size(m_parent.size().w, height));
    }
    else
    {
        resize(Size(m_parent.size().w, 100));
    }
}

void ComboBoxPopup::show(bool center)
{
    smart_pos();
    ComboBoxPopupBase::show(center);
}

int ComboBoxPopup::handle(eventid event)
{
    auto ret = ComboBoxPopupBase::handle(event);

    switch (event)
    {
    case eventid::MOUSE_DOWN:
    {
        Point mouse = from_screen(event_mouse());

        if (!Rect::point_inside(mouse, box()))
        {
            // if any mouse click happens outside of us, hide
            hide();
        }

        break;
    }
    default:
        break;
    }

    return ret;
}

}

ComboBox::ComboBox(const item_array& items,
                   const Rect& rect,
                   alignmask align,
                   const Font& font,
                   widgetmask flags) noexcept
    : TextWidget("", rect, align, font, flags),
      m_items(items),
      m_popup(*this)
{
    set_boxtype(Theme::boxtype::border);
    flag_set(widgetmask::GRAB_MOUSE);
    palette().set(Palette::BG, Palette::GROUP_NORMAL, palette().color(Palette::LIGHT));
    palette().set(Palette::BG, Palette::GROUP_ACTIVE, palette().color(Palette::LIGHT));
}

void ComboBox::set_parent(Frame* parent)
{
    TextWidget::set_parent(parent);
    parent->add(&m_popup);
}

int ComboBox::handle(eventid event)
{
    auto ret = Widget::handle(event);

    switch (event)
    {
    case eventid::MOUSE_DOWN:
    {
        Point mouse = from_screen(event_mouse());

        if (Rect::point_inside(mouse, box()))
        {
            m_popup.show_modal();
        }

        break;
    }
    default:
        break;
    }

    return ret;
}

void ComboBox::set_select(uint32_t index)
{
    if (m_selected != index)
    {
        if (index < m_items.size())
        {
            m_selected = index;
            damage();
            invoke_handlers(eventid::PROPERTY_CHANGED);
        }
    }
}

void ComboBox::resize(const Size& s)
{
    TextWidget::resize(s);

    if (m_popup.visible())
        m_popup.smart_pos();
}

void ComboBox::move(const Point& point)
{
    TextWidget::move(point);

    if (m_popup.visible())
        m_popup.smart_pos();
}

void ComboBox::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    static const int OFFSET = 5;

    // box
    draw_box(painter);

    Rect m_down_rect(x() + w() - h() + OFFSET * 2,
                     y(),
                     h() - OFFSET * 2,
                     h());

    // draw a down arrow
    auto cr = painter.context();
    painter.set_color(palette().color(Palette::HIGHLIGHT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));
    cairo_move_to(cr.get(), m_down_rect.x + OFFSET, m_down_rect.y + m_down_rect.h / 3);
    cairo_line_to(cr.get(), m_down_rect.x + m_down_rect.w / 2, m_down_rect.y + (m_down_rect.h / 3 * 2));
    cairo_line_to(cr.get(), m_down_rect.x + m_down_rect.w - OFFSET, m_down_rect.y + m_down_rect.h / 3);
    painter.set_line_width(3.0);
    cairo_stroke(cr.get());

    // text
    painter.set_color(palette().color(Palette::TEXT, disabled() ? Palette::GROUP_DISABLED : Palette::GROUP_NORMAL));
    painter.set_font(font());
    auto textbox = box();
    textbox -= Size(m_down_rect.w, 0);
    painter.draw_text(textbox, m_items[m_selected], m_text_align, 5);
}

ComboBox::~ComboBox()
{
}

}
}
