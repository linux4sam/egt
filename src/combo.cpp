/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/combo.h"
#include "egt/detail/imagecache.h"
#include "egt/frame.h"
#include "egt/image.h"
#include "egt/painter.h"
#include "egt/window.h"
#include <memory>

namespace egt
{
inline namespace v1
{

auto POPUP_OFFSET = 5;

namespace detail
{

ComboBoxPopup::ComboBoxPopup(ComboBox& parent)
    : Popup(Size(parent.size().w, ListBox::item_height())),
      m_list(std::make_shared<ListBox>()),
      m_parent(parent)
{
    set_name("ComboBoxPopup" + std::to_string(m_widgetid));

    for (auto& item : parent.m_items)
    {
        auto p = std::make_shared<StringItem>(item);
        m_list->add_item(p);
    }

    m_list->set_align(alignmask::expand);

    add(m_list);

    m_list->on_event([this, &parent](eventid event)
    {
        ignoreparam(event);
        parent.set_select(m_list->selected());
        hide();

        return 1;
    }, {eventid::property_changed});
}

void ComboBoxPopup::smart_pos()
{
    move(m_parent.box().point() + Point(0, m_parent.size().h + POPUP_OFFSET));

    if (main_screen())
    {
        auto ss = main_screen()->size();
        auto origin = to_display(box().point());
        auto height =
            std::min(static_cast<default_dim_type>((ss.h - origin.y) / ListBox::item_height()),
                     static_cast<default_dim_type>(m_list->count())) * ListBox::item_height();

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
    Popup::show(center);
}

int ComboBoxPopup::handle(eventid event)
{
    auto ret = Popup::handle(event);

    switch (event)
    {
    case eventid::pointer_click:
    {
        Point mouse = from_display(event::pointer().point);

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
                   const Widget::flags_type& flags) noexcept
    : TextWidget("", rect, align, font, flags),
      m_items(items),
      m_popup(new detail::ComboBoxPopup(*this))
{
    set_name("ComboBox" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::border);
    ncflags().set(Widget::flag::grab_mouse);
    palette().set(Palette::ColorId::bg, Palette::GroupId::normal, palette().color(Palette::ColorId::light));
    palette().set(Palette::ColorId::bg, Palette::GroupId::active, palette().color(Palette::ColorId::light));
}

void ComboBox::set_parent(Frame* parent)
{
    TextWidget::set_parent(parent);
    parent->add(m_popup);
}

int ComboBox::handle(eventid event)
{
    auto ret = Widget::handle(event);

    switch (event)
    {
    case eventid::pointer_click:
    {
        Point mouse = from_display(event::pointer().point);

        if (Rect::point_inside(mouse, box()))
        {
            m_popup->show_modal();
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
            invoke_handlers(eventid::property_changed);
        }
    }
}

void ComboBox::resize(const Size& s)
{
    TextWidget::resize(s);

    if (m_popup->visible())
        m_popup->smart_pos();
}

void ComboBox::move(const Point& point)
{
    TextWidget::move(point);

    if (m_popup->visible())
        m_popup->smart_pos();
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
    painter.set(palette().color(Palette::ColorId::highlight, disabled() ? Palette::GroupId::disabled : Palette::GroupId::normal));
    cairo_move_to(cr.get(), m_down_rect.x + OFFSET, m_down_rect.y + m_down_rect.h / 3);
    cairo_line_to(cr.get(), m_down_rect.x + m_down_rect.w / 2, m_down_rect.y + (m_down_rect.h / 3 * 2));
    cairo_line_to(cr.get(), m_down_rect.x + m_down_rect.w - OFFSET, m_down_rect.y + m_down_rect.h / 3);
    painter.set_line_width(3.0);
    cairo_stroke(cr.get());

    // text
    auto textbox = box();
    textbox -= Size(m_down_rect.w, 0);

    painter.set(palette().color(Palette::ColorId::text, disabled() ? Palette::GroupId::disabled : Palette::GroupId::normal));
    painter.set(font());

    auto size = text_size(m_items[m_selected]);
    textbox = detail::align_algorithm(size,
                                      textbox,
                                      m_text_align,
                                      5);
    painter.draw(textbox.point());
    painter.draw(m_items[m_selected]);
}

ComboBox::~ComboBox()
{
}

}
}
