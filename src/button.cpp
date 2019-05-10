/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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
    : Button(text, Rect())
{

}

Button::Button(const std::string& text, const Rect& rect) noexcept
    : TextWidget(text, rect, alignmask::center)
{
    set_name("Button" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::blank_rounded);

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

int Button::handle(eventid event)
{
    auto ret = TextWidget::handle(event);

    switch (event)
    {
    case eventid::raw_pointer_down:
    {
        set_active(true);
        return 1;
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

    return ret;
}

void Button::draw(Painter& painter, const Rect& rect)
{
    Drawer<Button>::draw(*this, painter, rect);
}

void Button::default_draw(Button& widget, Painter& painter, const Rect&)
{
    widget.draw_box(painter, Palette::ColorId::button_bg, Palette::ColorId::border);

    const auto b = widget.content_area();

    // text
    painter.set(widget.color(Palette::ColorId::button_text).color());
    painter.set(widget.font());
    auto size = painter.text_size(widget.text());
    Rect target = detail::align_algorithm(size,
                                          b,
                                          widget.text_align());
    painter.draw(target.point());
    painter.draw(widget.text());
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
            invoke_handlers(eventid::property_changed);
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

    widget.draw_box(painter, Palette::ColorId::button_bg, Palette::ColorId::border);

    if (!widget.text().empty())
    {
        auto text_size = widget.text_size(widget.text());

        Rect tbox;
        Rect ibox;

        if (widget.m_position_image_first)
            detail::double_align(widget.box(),
                                 widget.image().size(), widget.image_align(), ibox,
                                 text_size, widget.text_align(), tbox, 5);
        else
            detail::double_align(widget.box(),
                                 text_size, widget.text_align(), tbox,
                                 widget.image().size(), widget.image_align(), ibox, 5);

        //image
        painter.draw(ibox.point());
        painter.draw(widget.image());

        //text
        painter.set(widget.color(Palette::ColorId::button_text).color());
        painter.set(widget.font());
        painter.draw(tbox.point());
        painter.draw(widget.text());
    }
    else
    {
        Rect target = detail::align_algorithm(widget.image().size(), widget.box(),
                                              widget.image_align());
        painter.draw(target.point());
        painter.draw(widget.image());
    }
}

ImageButton::~ImageButton()
{

}

void ImageButton::first_resize()
{
    if (box().size().empty())
    {
        if (!m_text.empty())
        {
            auto text_size = this->text_size(m_text);

            Rect tbox;
            Rect ibox;

            if (m_position_image_first)
                detail::double_align(box(),
                                     m_image.size(), m_image_align, ibox,
                                     text_size, m_text_align, tbox, 5);
            else
                detail::double_align(box(),
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

namespace experimental
{
HotSpot::HotSpot(const Rect& rect) noexcept
    : Widget(rect)
{
    set_boxtype(Theme::boxtype::none);
    hide();
}

HotSpot::
HotSpot(Frame& parent, const Rect& rect) noexcept
    : HotSpot(rect)
{
    parent.add(*this);
}
}

}
}
