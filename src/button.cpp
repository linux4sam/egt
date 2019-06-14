/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/button.h"
#include "egt/detail/alignment.h"
#include "egt/detail/imagecache.h"
#include "egt/frame.h"
#include "egt/painter.h"
#include "egt/theme.h"
#include "egt/widget.h"

using namespace std;

namespace egt
{
inline namespace v1
{
static const auto DEFAULT_BUTTON_SIZE = Size(100, 30);

Button::Button(const std::string& text) noexcept
    : Button(text, {})
{}

Button::Button(const std::string& text, const Rect& rect) noexcept
    : TextWidget(text, rect, alignmask::center)
{
    set_name("Button" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::blank_rounded);
    set_padding(2);

    ncflags().set(Widget::flag::grab_mouse);
}

Button::Button(Frame& parent, const std::string& text) noexcept
    : Button(text)
{
    parent.add(*this);
}

Button::Button(Frame& parent, const std::string& text, const Rect& rect) noexcept
    : Button(text, rect)
{
    parent.add(*this);
}

void Button::handle(Event& event)
{
    TextWidget::handle(event);

    switch (event.id())
    {
    case eventid::raw_pointer_down:
    {
        set_active(true);
        break;
    }
    case eventid::raw_pointer_up:
    {
        set_active(false);
        break;
    }
    default:
        break;
    }
}

void Button::draw(Painter& painter, const Rect& rect)
{
    Drawer<Button>::draw(*this, painter, rect);
}

void Button::default_draw(Button& widget, Painter& painter, const Rect&)
{
    widget.draw_box(painter, Palette::ColorId::button_bg, Palette::ColorId::border);

    detail::draw_text(painter,
                      widget.content_area(),
                      widget.text(),
                      widget.font(),
                      TextBox::flags_type({TextBox::flag::multiline, TextBox::flag::word_wrap}),
                      widget.text_align(),
                      justification::middle,
                      widget.color(Palette::ColorId::button_text).color());
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

        /* Check if the button group has not canceled the change. */
        if (m_checked == value)
        {
            damage();
            Event event(eventid::property_changed);
            invoke_handlers(event);
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

Size Button::min_size_hint() const
{
    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        s *= Size(1, 3);
        s += Size(s.w / 2 + 5, 0);
        return s + Widget::min_size_hint();
    }

    return DEFAULT_BUTTON_SIZE + Widget::min_size_hint();
}

void Button::first_resize()
{
    if (box().size().empty())
    {
        resize(min_size_hint());
    }
}

ImageButton::ImageButton(const Image& image,
                         const std::string& text,
                         const Rect& rect) noexcept
    : Button(text, rect)
{
    set_name("ImageButton" + std::to_string(m_widgetid));

    if (text.empty())
        set_image_align(alignmask::center);
    do_set_image(image);
}

ImageButton::ImageButton(Frame& parent,
                         const Image& image,
                         const std::string& text,
                         const Rect& rect) noexcept
    : ImageButton(image, text, rect)
{
    parent.add(*this);
}

void ImageButton::do_set_image(const Image& image)
{
    if (!image.empty())
    {
        m_image = image;
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

    widget.draw_box(painter, Palette::ColorId::button_bg, Palette::ColorId::border);

    if (!widget.text().empty())
    {
        if (!widget.image().empty())
        {
            detail::draw_text(painter,
                              widget.content_area(),
                              widget.text(),
                              widget.font(),
                              TextBox::flags_type({TextBox::flag::multiline, TextBox::flag::word_wrap}),
                              widget.text_align(),
                              justification::middle,
                              widget.color(Palette::ColorId::button_text).color(),
                              widget.image_align(),
                              widget.image());
        }
        else
        {
            detail::draw_text(painter,
                              widget.content_area(),
                              widget.text(),
                              widget.font(),
                              TextBox::flags_type({TextBox::flag::multiline, TextBox::flag::word_wrap}),
                              widget.text_align(),
                              justification::middle,
                              widget.color(Palette::ColorId::button_text).color());
        }
    }
    else if (!widget.image().empty())
    {
        Rect target = detail::align_algorithm(widget.image().size(), widget.box(),
                                              widget.image_align());
        painter.draw(target.point());
        painter.draw(widget.image());
    }
}

void ImageButton::first_resize()
{
    if (box().size().empty())
    {
        resize(m_image.size());
    }
}

}
}
