/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/button.h"
#include "egt/detail/alignment.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/meta.h"
#include "egt/frame.h"
#include "egt/painter.h"
#include "egt/serialize.h"
#include "egt/theme.h"
#include "egt/widget.h"

namespace egt
{
inline namespace v1
{
static Size default_button_size_value = Size(100, 30);

Size Button::default_size()
{
    return default_button_size_value;
}

void Button::default_size(const Size& size)
{
    default_button_size_value = size;
}

static AlignFlags default_text_align_value{AlignFlag::center};

AlignFlags Button::default_text_align()
{
    return default_text_align_value;
}

void Button::default_text_align(const AlignFlags& align)
{
    default_text_align_value = align;
}

Button::Button(const std::string& text, const AlignFlags& text_align) noexcept
    : Button(text, {}, text_align)
{}

Button::Button(const std::string& text,
               const Rect& rect,
               const AlignFlags& text_align) noexcept
    : TextWidget(text, rect, text_align)
{
    name("Button" + std::to_string(m_widgetid));

    fill_flags(Theme::FillFlag::blend);
    border_radius(4.0);

    grab_mouse(true);
}

Button::Button(Frame& parent,
               const std::string& text,
               const AlignFlags& text_align) noexcept
    : Button(text, text_align)
{
    parent.add(*this);
}

Button::Button(Frame& parent,
               const std::string& text,
               const Rect& rect,
               const AlignFlags& text_align) noexcept
    : Button(text, rect, text_align)
{
    parent.add(*this);
}

void Button::handle(Event& event)
{
    TextWidget::handle(event);

    switch (event.id())
    {
    case EventId::raw_pointer_down:
    {
        active(true);
        break;
    }
    case EventId::raw_pointer_up:
    {
        active(false);
        break;
    }
    default:
        break;
    }
}

void Button::text(const std::string& text)
{
    if (detail::change_if_diff<>(m_text, text))
    {
        on_text_changed.invoke();
        damage();
        layout();
    }
}

void Button::draw(Painter& painter, const Rect& rect)
{
    Drawer<Button>::draw(*this, painter, rect);
}

void Button::default_draw(Button& widget, Painter& painter, const Rect& /*rect*/)
{
    widget.draw_box(painter, Palette::ColorId::button_bg, Palette::ColorId::border);

    detail::draw_text(painter,
                      widget.content_area(),
                      widget.text(),
                      widget.font(),
                      TextBox::TextFlags({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap}),
                      widget.text_align(),
                      Justification::middle,
                      widget.color(Palette::ColorId::button_text));
}

void Button::checked(bool value)
{
    if (flags().is_set(Widget::Flag::checked) != value)
    {
        if (value)
            flags().set(Widget::Flag::checked);
        else
            flags().clear(Widget::Flag::checked);

        if (m_group)
            m_group->checked_state_change(*this, value);

        /* Check if the button group has not canceled the change. */
        if (flags().is_set(Widget::Flag::checked) == value)
        {
            damage();
            on_checked_changed.invoke();
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
    if (!m_min_size.empty())
        return m_min_size;

    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        // add a little bit of fluff for touch
        s *= Size(1, 3);
        s += Size(s.width() / 2 + 5, 0);
        return s + Widget::min_size_hint();
    }

    return default_size() + Widget::min_size_hint();
}

Button::~Button()
{
    if (m_group)
        m_group->remove(this);
}

ImageButton::ImageButton(const std::string& text,
                         const AlignFlags& text_align) noexcept
    : ImageButton(Image(), text, text_align)
{}

ImageButton::ImageButton(const Image& image,
                         const std::string& text,
                         const AlignFlags& text_align) noexcept
    : ImageButton(image, text, {}, text_align)
{}

ImageButton::ImageButton(const Image& image,
                         const std::string& text,
                         const Rect& rect,
                         const AlignFlags& text_align) noexcept
    : Button(text, rect, text_align)
{
    name("ImageButton" + std::to_string(m_widgetid));

    if (text.empty())
        image_align(AlignFlag::center);
    do_set_image(image);
}

ImageButton::ImageButton(Frame& parent,
                         const Image& image,
                         const std::string& text,
                         const AlignFlags& text_align) noexcept
    : ImageButton(image, text, text_align)
{
    parent.add(*this);
}

ImageButton::ImageButton(Frame& parent,
                         const Image& image,
                         const std::string& text,
                         const Rect& rect,
                         const AlignFlags& text_align) noexcept
    : ImageButton(image, text, rect, text_align)
{
    parent.add(*this);
}

Size ImageButton::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    Rect size = Button::min_size_hint() - Size(moat() * 2, moat() * 2);

    if (!m_image.size().empty())
    {
        if (image_align().is_set(AlignFlag::left) ||
            image_align().is_set(AlignFlag::right))
        {
            size += Size(m_image.width(), 0);
        }
        else if (image_align().is_set(AlignFlag::top) ||
                 image_align().is_set(AlignFlag::bottom))
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

void ImageButton::image(const Image& image)
{
    do_set_image(image);
}

void ImageButton::draw(Painter& painter, const Rect& rect)
{
    Drawer<ImageButton>::draw(*this, painter, rect);
}

void ImageButton::default_draw(ImageButton& widget, Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

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
                              TextBox::TextFlags({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap}),
                              widget.text_align(),
                              Justification::middle,
                              widget.color(Palette::ColorId::button_text),
                              widget.image_align(),
                              widget.image());
        }
        else
        {
            detail::draw_text(painter,
                              widget.content_area(),
                              text,
                              widget.font(),
                              TextBox::TextFlags({TextBox::TextFlag::multiline, TextBox::TextFlag::word_wrap}),
                              widget.text_align(),
                              Justification::middle,
                              widget.color(Palette::ColorId::button_text));
        }
    }
    else if (!widget.image().empty())
    {
        auto target = detail::align_algorithm(widget.image().size(),
                                              widget.content_area(),
                                              widget.image_align());
        painter.draw(target.point());
        painter.draw(widget.image());
    }
}

void ImageButton::show_label(bool value)
{
    if (detail::change_if_diff<>(m_show_label, value))
        damage();
}

void ImageButton::serialize(Serializer& serializer) const
{
    Button::serialize(serializer);

    serializer.add_property("showlabel", show_label());
    m_image.serialize("image", serializer);

    // TODO m_image_align
}

void ImageButton::deserialize(const std::string& name, const std::string& value,
                              const Serializer::Attributes& attrs)
{
    // TODO proper loading of all image properties

    switch (detail::hash(name))
    {
    case detail::hash("showlabel"):
        show_label(std::stoul(value));
        break;
    case detail::hash("image"):
        m_image.deserialize(name, value, attrs);
        break;
    default:
        Button::deserialize(name, value, attrs);
        break;
    }
}

}
}
