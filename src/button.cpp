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

Button::Button(const std::string& text, const Rect& rect) noexcept
    : TextWidget(text, rect, alignmask::center)
{
    set_name("Button" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::blank_rounded);

    ncflags().set(Widget::flag::grab_mouse);
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

void Button::set_text(const std::string& text)
{
    if (detail::change_if_diff<>(m_text, text))
    {
        damage();
        layout();
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

void Button::set_checked(bool value)
{
    if (detail::change_if_diff<>(m_checked, value))
    {
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

void Button::set_parent(Frame* parent)
{
    TextWidget::set_parent(parent);
    layout();
}

Size Button::min_size_hint() const
{
    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        // add a little bit of fluff for touch
        s *= Size(1, 3);
        s += Size(s.width() / 2 + 5, 0);
        return s + Widget::min_size_hint();
    }

    return DEFAULT_BUTTON_SIZE + Widget::min_size_hint();
}

Button::~Button()
{
    if (m_group)
        m_group->remove(*this);
}

ImageButton::ImageButton(const std::string& text,
                         const Rect& rect) noexcept
    : ImageButton(Image(), text, rect)
{
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

Size ImageButton::min_size_hint() const
{
    Rect size = Button::min_size_hint() - Size(moat() * 2, moat() * 2);

    if (!m_image.size().empty())
    {
        if ((image_align() & alignmask::left) == alignmask::left ||
            (image_align() & alignmask::right) == alignmask::right)
        {
            size += Size(m_image.width(), 0);
        }
        else if ((image_align() & alignmask::top) == alignmask::top ||
                 (image_align() & alignmask::bottom) == alignmask::bottom)
        {
            size += Size(0, m_image.height());
        }

        size = Rect::merge(size, m_image.size());
    }

    auto res = size.size() + Size(moat() * 2, moat() * 2);
    return res;
}

void ImageButton::do_set_image(const Image& image)
{
    if (size().empty() && !image.empty())
        resize(image.size());

    m_image = image;
    damage();
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
        std::string text;
        if (widget.show_label())
            text = widget.text();

        if (!widget.image().empty())
        {
            detail::draw_text(painter,
                              widget.content_area(),
                              text,
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
                              text,
                              widget.font(),
                              TextBox::flags_type({TextBox::flag::multiline, TextBox::flag::word_wrap}),
                              widget.text_align(),
                              justification::middle,
                              widget.color(Palette::ColorId::button_text).color());
        }
    }
    else if (!widget.image().empty())
    {
        Rect target = detail::align_algorithm(widget.image().size(),
                                              widget.content_area(),
                                              widget.image_align());
        painter.draw(target.point());
        painter.draw(widget.image());
    }
}

void ImageButton::set_show_label(bool value)
{
    if (detail::change_if_diff<>(m_show_label, value))
        damage();
}

}
}
