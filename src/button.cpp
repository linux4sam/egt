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
#include "egt/detail/layout.h"
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

void Button::set_parent(Widget* parent)
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
        if (m_text.find('\n') == m_text.npos)
        {
            // add a little bit of fluff for touch
            s *= Size(1, 3);
            s += Size(s.width() / 2 + 5, 0);
        }
        else
        {
            /*
             * If the text is multiline, don't multiply the height by 3, it's
             * too much, just add a little bit of extra space.
             */
            s += Size(s.width() / 2 + 5, 5);
        }
        return s + Widget::min_size_hint();
    }

    return default_size() + Widget::min_size_hint();
}

Button::~Button() noexcept
{
    if (m_group)
        m_group->remove(this);
}

Switch::Switch(const std::string& text,
               const Rect& rect) noexcept
    : Button(text, rect)
{
    name("Switch" + std::to_string(m_widgetid));

    fill_flags().clear();
    padding(5);
    text_align(AlignFlag::left | AlignFlag::center_vertical);

    grab_mouse(true);
}

Switch::Switch(Serializer::Properties& props, bool is_derived) noexcept
    : Button(props, true)
{
    deserialize(props);

    if (!is_derived)
        deserialize_leaf(props);
}

void Switch::serialize(Serializer& serializer) const
{
    Button::serialize(serializer);

    serializer.add_property("show_label", show_label());
    if (!switch_align().empty())
        serializer.add_property("switch_align", switch_align());
    if (m_normal_switch)
        serializer.add_property("normal_switch", m_normal_switch->uri());
    if (m_checked_switch)
        serializer.add_property("checked_switch", m_checked_switch->uri());
}

void Switch::deserialize(Serializer::Properties& props)
{
    props.erase(std::remove_if(props.begin(), props.end(), [&](auto & p)
    {
        auto value = std::get<1>(p);
        switch (detail::hash(std::get<0>(p)))
        {
        case detail::hash("show_label"):
            show_label(egt::detail::from_string(std::get<1>(p)));
            break;
        case detail::hash("switch_align"):
            switch_align(AlignFlags(std::get<1>(p)));
            break;
        case detail::hash("normal_switch"):
            switch_image(Image(value), false);
            break;
        case detail::hash("checked_switch"):
            switch_image(Image(value), true);
            break;
        default:
            return false;
        }
        return true;
    }), props.end());
}

void Switch::draw(Painter& painter, const Rect& rect)
{
    Drawer<Switch>::draw(*this, painter, rect);
}

void Switch::default_draw(const Switch& widget, Painter& painter, const Rect& rect)
{
    detail::ignoreparam(rect);

    widget.draw_box(painter, Palette::ColorId::label_bg, Palette::ColorId::border);

    auto b = widget.content_area();
    std::vector<detail::LayoutRect> rects;
    DefaultDim length;
    Rect handle;

    if (widget.show_label())
    {
        painter.set(widget.font());
        auto text_size = painter.text_size(widget.text());

        Rect text;
        if (widget.switch_align().is_set(AlignFlag::right) ||
            widget.switch_align().is_set(AlignFlag::left))
        {
            length = std::min<DefaultDim>(b.width() - text_size.width(), b.height());
            if (length < 0)
                length = b.width() * 0.15;

            if (widget.switch_align().is_set(AlignFlag::right))
            {
                rects.emplace_back(0,
                                   Rect(0, 0, b.width() - length, b.height()),
                                   widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, length, length),
                                   0, 0, widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::horizontal);

                text = rects[0].rect + b.point();
                handle = rects[1].rect + b.point();
            }
            else
            {
                rects.emplace_back(0,
                                   Rect(0, 0, length, length),
                                   0, 0, widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, b.width() - length, b.height()),
                                   widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::horizontal);

                handle = rects[0].rect + b.point();
                text = rects[1].rect + b.point();
            }
        }
        else
        {
            length = std::min<DefaultDim>(b.height() - text_size.height(), b.width());
            if (length < 0)
                length = b.height() * 0.15;

            if (widget.switch_align().is_set(AlignFlag::bottom))
            {
                rects.emplace_back(0,
                                   Rect(0, 0, b.width(), b.height() - length),
                                   widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, length, length),
                                   0, 0, widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::vertical);

                text = rects[0].rect + b.point();
                handle = rects[1].rect + b.point();
            }
            else
            {
                rects.emplace_back(0,
                                   Rect(0, 0, length, length),
                                   0, 0, widget.padding() / 2);
                rects.emplace_back(0,
                                   Rect(0, 0, b.width(), b.height() - length),
                                   widget.padding() / 2);

                detail::flex_layout(b, rects, Justification::start, Orientation::vertical);

                handle = rects[0].rect + b.point();
                text = rects[1].rect + b.point();
            }
        }

        // text
        painter.set(widget.color(Palette::ColorId::label_text));
        Rect target = detail::align_algorithm(text_size,
                                              text,
                                              widget.text_align());
        painter.draw(target.point());
        painter.draw(widget.text());
    }
    else
    {
        length = std::min<DefaultDim>(b.width(), b.height());
        if (length < 0)
            length = b.width() * 0.15;

        rects.emplace_back(0,
                           Rect(0, 0, length, length),
                           0, 0, widget.padding() / 2);

        detail::flex_layout(b, rects, Justification::middle, Orientation::horizontal);

        handle = rects[0].rect + b.point();
    }

    widget.draw_switch(painter, handle);
}

void Switch::draw_switch(Painter& painter, const Rect& handle) const
{
    auto* image = checked() ? m_checked_switch.get() : m_normal_switch.get();
    if (!image)
        return;

    theme().draw_box(painter, Theme::FillFlag::blend, handle,
                     Palette::transparent, Palette::transparent,
                     0, 0, 0, {}, image);
}

void Switch::handle(Event& event)
{
    // NOLINTNEXTLINE(bugprone-parent-virtual-call)
    Widget::handle(event);

    switch (event.id())
    {
    case EventId::pointer_click:
        checked(!checked());
    default:
        break;
    }
}

Size Switch::min_size_hint() const
{
    if (!m_min_size.empty())
        return m_min_size;

    auto s = Size(1, 1);
    if (!m_text.empty())
    {
        s = text_size(m_text);
        if (m_switch_align.empty() ||
            m_switch_align.is_set(AlignFlag::left) ||
            m_switch_align.is_set(AlignFlag::right))
        {
            s += Size(s.height(), 0);
        }
        else if (m_switch_align.is_set(AlignFlag::top) ||
                 m_switch_align.is_set(AlignFlag::bottom))
        {
            s += Size(0, s.height());
        }
    }

    return s + Widget::min_size_hint();
}

void Switch::text(const std::string& text)
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

Image* Switch::switch_image(bool checked) const
{
    auto& btn = checked ? m_checked_switch : m_normal_switch;

    return btn.get();
}

void Switch::switch_image(const Image& image, bool checked)
{
    auto& btn = checked ? m_checked_switch : m_normal_switch;

    btn = std::make_unique<Image>(image);
    if (checked == this->checked())
        damage();
}

void Switch::reset_switch_image(bool checked)
{
    auto& btn = checked ? m_checked_switch : m_normal_switch;

    if (btn.get())
    {
        btn.reset();

        if (checked == this->checked())
            damage();
    }
}

}
}
