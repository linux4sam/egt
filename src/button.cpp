/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/app.h"
#include "egt/button.h"
#include "egt/detail/alignment.h"
#include "egt/detail/imagecache.h"
#include "egt/detail/meta.h"
#include "egt/detail/screen/composerscreen.h"
#include "egt/frame.h"
#include "egt/painter.h"
#include "egt/serialize.h"
#include "egt/theme.h"
#include "egt/widget.h"

namespace egt
{
inline namespace v1
{

Size Button::default_button_size_value;
Signal<>::RegisterHandle Button::default_button_size_handle = Signal<>::INVALID_HANDLE;
AlignFlags Button::default_text_align_value = AlignFlag::center;

void Button::register_handler()
{
    if (default_button_size_handle == Signal<>::INVALID_HANDLE)
    {
        default_button_size_handle = detail::ComposerScreen::register_screen_resize_hook([]()
        {
            default_button_size_value.clear();
        });
    }
}

void Button::unregister_handler()
{
    detail::ComposerScreen::unregister_screen_resize_hook(default_button_size_handle);
    default_button_size_handle = Signal<>::INVALID_HANDLE;
}

Size Button::default_size()
{
    if (default_button_size_value.empty())
    {
        register_handler();
        default_button_size_value =  egt::Application::instance().screen()->size() * 0.12;
    }

    return default_button_size_value;
}

void Button::default_size(const Size& size)
{
    if (!size.empty())
        unregister_handler();

    default_button_size_value = size;
}

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

Button::Button(Serializer::Properties& props, bool is_derived) noexcept
    : TextWidget(props, true)
{
    name("Button" + std::to_string(m_widgetid));

    if (!is_derived)
        deserialize_leaf(props);
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

void Button::default_draw(const Button& widget, Painter& painter, const Rect& /*rect*/)
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

Button::~Button() noexcept
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
    : Button(text, rect, text_align),
      ImageHolder(*static_cast<Widget*>(this))
{
    name("ImageButton" + std::to_string(m_widgetid));

    if (text.empty())
    {
        show_label(false);
        image_align(AlignFlag::center | AlignFlag::expand);
    }
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

ImageButton::ImageButton(Serializer::Properties& props, bool is_derived) noexcept
    : Button(props, true),
      ImageHolder(*static_cast<Widget*>(this))
{
    name("ImageButton" + std::to_string(m_widgetid));

    if (m_text.empty())
        image_align(AlignFlag::center);

    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

Size ImageButton::min_size_hint() const
{
    return ImageHolder::min_size_hint();
}

void ImageButton::draw(Painter& painter, const Rect& rect)
{
    Drawer<ImageButton>::draw(*this, painter, rect);
}

void ImageButton::default_draw(ImageButton& widget, Painter& painter, const Rect& rect)
{
    widget.ImageHolder::default_draw(painter,
                                     rect,
                                     Palette::ColorId::button_bg,
                                     Palette::ColorId::border,
                                     Palette::ColorId::button_text);
}

void ImageButton::text(const std::string& text)
{
    if (m_text != text)
    {
        if (text.empty())
            show_label(false);
        else
            show_label(true);
    }

    Button::text(text);
}

void ImageButton::serialize(Serializer& serializer) const
{
    ImageHolder::serialize(serializer);
}

void ImageButton::deserialize(Serializer::Properties& props)
{
    ImageHolder::deserialize(props);
}

}
}
