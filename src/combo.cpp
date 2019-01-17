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

#define CSWITCH(x) x
//#define CSWITCH(x) to_screen((x))

ComboBoxPopup::ComboBoxPopup(ComboBox& parent)
    : Popup<BasicWindow>(Size(parent.size().w,
                              ListBox::item_height() * parent.m_items.size()))
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

int ComboBoxPopup::handle(eventid event)
{
    auto ret = Popup<BasicWindow>::handle(event);

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
    set_boxtype(Theme::boxtype::rounded_border);
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
            m_popup.move(CSWITCH(box().point() + Point(0, size().h + POPUP_OFFSET)));
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

    m_popup.resize(Size(s.w, m_popup.size().h));
}

void ComboBox::move(const Point& point)
{
    TextWidget::move(point);

    m_popup.move(CSWITCH(box().point() + Point(0, size().h + POPUP_OFFSET)));
}

void ComboBox::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    // box
    draw_box(painter);

    // images
    auto OFFSET = 5;

    auto downsize = Size(h() - OFFSET * 2, h() - OFFSET * 2);

    auto scale = static_cast<float>(downsize.w) /
                 static_cast<float>(Painter::surface_to_size(detail::image_cache().get("down.png")).w);

    auto down = Image("down.png", scale, scale);

    auto m_down_rect = Rect(Point(x() + w() - downsize.w - OFFSET,
                                  y() + OFFSET),
                            downsize);

    painter.draw_image(m_down_rect.point(), down);

    // text
    painter.set_color(palette().color(Palette::TEXT));
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
