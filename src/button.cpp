/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/button.h"
#include "egt/imagecache.h"
#include "egt/painter.h"
#include "egt/frame.h"
#include "egt/widget.h"
#include "egt/theme.h"

using namespace std;

namespace egt
{
inline namespace v1
{
static const auto DEFAULT_BUTTON_SIZE = Size(100, 50);

Button::Button(const std::string& text, const Rect& rect,
               const Font& font, widgetmask flags) noexcept
    : TextWidget(text, rect, alignmask::CENTER, font, flags)
{
    set_boxtype(Theme::boxtype::rounded_gradient);
    palette().set(Palette::BG, Palette::GROUP_NORMAL, palette().color(Palette::HIGHLIGHT));
    flag_set(widgetmask::GRAB_MOUSE);
}

Button::Button(Frame& parent, const std::string& text, const Rect& rect,
               const Font& font, widgetmask flags) noexcept
    : Button(text, rect, font, flags)
{
    parent.add(this);
}

Button::Button(Frame& parent, const std::string& text,
               const Font& font, widgetmask flags) noexcept
    : Button(parent, text, Rect(), font, flags)
{
}

int Button::handle(eventid event)
{
    auto ret = TextWidget::handle(event);

    switch (event)
    {
    case eventid::RAW_POINTER_DOWN:
    {
        set_active(true);
        break;
    }
    case eventid::RAW_POINTER_UP:
    {
        set_active(false);
        break;
    }
    default:
        break;
    }

    return ret;
}

void Button::draw(Painter& painter, const Rect& rect)
{
    Drawer<Button>::draw(*this, painter, rect);
}

void Button::default_draw(Button& widget, Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    widget.draw_box(painter);

    // text
    painter.set_color(widget.palette().color(Palette::ColorId::TEXT));
    painter.set_font(widget.font());
    painter.draw_text(widget.box(), widget.text(), widget.text_align(), 5);
}

bool Button::checked() const
{
    return m_checked;
}

void Button::check(bool value)
{
    if (m_checked != value)
    {
        m_checked = value;
        if (m_group)
            m_group->checked_state_change(*this, value);

        /* Check if the button group has not cancelled the change. */
        if (m_checked == value)
        {
            damage();
            invoke_handlers(eventid::PROPERTY_CHANGED);
        }
    }
}

Button::~Button()
{
    if (m_group)
        m_group->remove(*this);
}

void Button::set_parent(Frame* parent)
{
    TextWidget::set_parent(parent);
    first_resize();
}

void Button::first_resize()
{
    if (box().size().empty())
    {
        if (!m_text.empty())
        {
            auto s = text_size();
            s += Size(10, 10);
            resize(s);
        }
        else
        {
            resize(DEFAULT_BUTTON_SIZE);
        }
    }
}

ImageButton::ImageButton(const Image& image,
                         const std::string& text,
                         const Rect& rect,
                         widgetmask flags) noexcept
    : Button(text, rect, Font(), flags)
{
    if (text.empty())
        set_image_align(alignmask::CENTER);
    do_set_image(image);
}

ImageButton::ImageButton(Frame& parent,
                         const Image& image,
                         const std::string& text,
                         const Rect& rect,
                         widgetmask flags) noexcept
    : ImageButton(image, text, rect, flags)
{
    parent.add(this);
}

void ImageButton::do_set_image(const Image& image)
{
    if (!image.empty())
    {
        m_image = image;
#if 0
        auto width = cairo_image_surface_get_width(m_image.get());
        auto height = cairo_image_surface_get_height(m_image.get());
        m_box.w = width;
        m_box.h = height;
#endif
        damage();
    }
}

void ImageButton::set_image(const Image& image)
{
    do_set_image(image);
}

void ImageButton::draw(Painter& painter, const Rect& rect)
{
    Drawer<ImageButton>::draw(*this, painter, rect);
}

void ImageButton::default_draw(ImageButton& widget, Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    widget.draw_box(painter);

    if (!widget.text().empty())
    {
        auto text_size = widget.text_size();

        Rect tbox;
        Rect ibox;

        if (widget.m_position_image_first)
            Widget::double_align(widget.box(),
                                 widget.image().size(), widget.image_align(), ibox,
                                 text_size, widget.text_align(), tbox, 5);
        else
            Widget::double_align(widget.box(),
                                 text_size, widget.text_align(), tbox,
                                 widget.image().size(), widget.image_align(), ibox, 5);

        painter.draw_image(ibox.point(), widget.image(), widget.disabled());

        painter.draw_text(widget.text(), tbox, widget.palette().color(Palette::TEXT),
                          alignmask::CENTER, 0, widget.font());
    }
    else
    {
        Rect target = Widget::align_algorithm(widget.image().size(), widget.box(),
                                              widget.image_align(), 0);
        painter.draw_image(target.point(), widget.image(), widget.disabled());
    }
}

ImageButton::~ImageButton()
{}

void ImageButton::first_resize()
{
    if (box().size().empty())
    {
        if (!m_text.empty())
        {
            auto text_size = this->text_size();

            Rect tbox;
            Rect ibox;

            if (m_position_image_first)
                Widget::double_align(box(),
                                     m_image.size(), m_image_align, ibox,
                                     text_size, m_text_align, tbox, 5);
            else
                Widget::double_align(box(),
                                     text_size, m_text_align, tbox,
                                     m_image.size(), m_image_align, ibox, 5);

            auto s = Rect::merge(tbox, ibox);
            resize(s.size() + Size(10, 10));
        }
        else
        {
            resize(m_image.size());
        }
    }
}

}
}
