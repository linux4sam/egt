/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/canvas.h"
#include "egt/detail/alignment.h"
#include "egt/detail/enum.h"
#include "egt/detail/layout.h"
#include "egt/detail/string.h"
#include "egt/frame.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/text.h"
#include "egt/virtualkeyboard.h"

namespace egt
{
inline namespace v1
{

const AlignFlags TextBox::default_align = AlignFlag::expand;

TextBox::TextBox(const std::string& text,
                 const AlignFlags& text_align,
                 const TextFlags& flags) noexcept
    : TextBox(text, {}, text_align, flags)
{}

TextBox::TextBox(const std::string& text,
                 const Rect& rect,
                 const AlignFlags& text_align,
                 const TextFlags& flags) noexcept
    : TextWidget(text, rect, text_align),
      m_timer(std::chrono::seconds(1)),
      m_text_flags(flags)
{
    name("TextBox" + std::to_string(m_widgetid));

    border(theme().default_border());
    boxtype({Theme::BoxFlag::fill, Theme::BoxFlag::border_rounded});
    padding(5);

    m_timer.on_timeout([this]() { cursor_timeout(); });

    cursor_end();
}

TextBox::TextBox(Frame& parent,
                 const std::string& text,
                 const AlignFlags& text_align,
                 const TextFlags& flags) noexcept
    : TextBox(text, Rect(), text_align, flags)
{
    parent.add(*this);
}

TextBox::TextBox(Frame& parent,
                 const std::string& text,
                 const Rect& rect,
                 const AlignFlags& text_align,
                 const TextFlags& flags) noexcept
    : TextBox(text, rect, text_align, flags)
{
    parent.add(*this);
}

void TextBox::handle(Event& event)
{
    Widget::handle(event);

    switch (event.id())
    {
    case EventId::on_gain_focus:
        show_cursor();
        break;
    case EventId::on_lost_focus:
        hide_cursor();
        if (!text_flags().is_set(TextFlag::no_virt_keyboard))
            if (popup_virtual_keyboard())
                popup_virtual_keyboard()->hide();
        break;
    case EventId::pointer_click:
        keyboard_focus(this);
        if (!text_flags().is_set(TextFlag::no_virt_keyboard))
            if (popup_virtual_keyboard())
                popup_virtual_keyboard()->show();
        break;
    case EventId::keyboard_down:
        handle_key(event.key());
        event.stop();
    default:
        break;
    }
}

void TextBox::handle_key(const Key& key)
{
    switch (key.keycode)
    {
    case EKEY_ESCAPE:
        // do nothing
        break;
    case EKEY_BACKSPACE:
    {
        if (cursor())
        {
            selection(cursor() - 1, 1);
            delete_selection();
        }
        break;
    }
    case EKEY_DELETE:
    {
        selection(cursor(), 1);
        delete_selection();
        break;
    }
    case EKEY_ENTER:
    {
        if (text_flags().is_set(TextFlag::multiline))
            insert("\n");
        break;
    }
    case EKEY_LEFT:
    {
        cursor_backward();
        break;
    }
    case EKEY_RIGHT:
    {
        cursor_forward();
        break;
    }
    case EKEY_UP:
    {
        if (text_flags().is_set(TextFlag::multiline))
        {
            // TODO
        }
        break;
    }
    case EKEY_DOWN:
    {
        if (text_flags().is_set(TextFlag::multiline))
        {
            // TODO
        }
        break;
    }
    case EKEY_END:
    {
        // TODO
        break;
    }
    case EKEY_HOME:
    {
        // TODO
        break;
    }
    default:
    {
        if (key.unicode)
        {
            std::string tmp;
            utf8::append(key.unicode, std::back_inserter(tmp));
            insert(tmp);
        }
        break;
    }
    }
}

auto CURSOR_Y_OFFSET = 2.;

void TextBox::draw(Painter& painter, const Rect&)
{
    // box
    draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    // function to draw the cursor
    auto draw_cursor = [&](const Point & offset, size_t height)
    {
        if (focus() && m_cursor_state)
        {
            painter.set(color(Palette::ColorId::cursor).color());
            painter.line_width(2);

            painter.draw(offset + Point(0, -CURSOR_Y_OFFSET),
                         offset + Point(0, height) + Point(0, CURSOR_Y_OFFSET));
            painter.stroke();
        }
    };

    detail::draw_text(painter,
                      content_area(),
                      m_text,
                      font(),
                      text_flags(),
                      text_align(),
                      Justification::start,
                      color(Palette::ColorId::text).color(),
                      draw_cursor,
                      m_cursor_pos,
                      color(Palette::ColorId::text_highlight).color(),
                      m_select_start,
                      m_select_len);
}

void TextBox::text(const std::string& str)
{
    clear_selection();
    clear();
    insert(str);
}

void TextBox::clear()
{
    clear_selection();
    cursor_begin();
    TextWidget::clear();
}

void TextBox::max_length(size_t len)
{
    if (detail::change_if_diff<>(m_max_len, len))
    {
        if (m_max_len)
        {
            auto len = detail::utf8len(m_text);
            if (len > m_max_len)
            {
                auto i = m_text.begin();
                utf8::advance(i, m_max_len, m_text.end());
                m_text.erase(i, m_text.end());
            }
        }

        damage();
    }
}

size_t TextBox::append(const std::string& str)
{
    cursor_end();
    return insert(str);
}

size_t TextBox::width_to_len(const std::string& text) const
{
    auto b = content_area();

    Canvas canvas(Size(100, 100));
    auto cr = canvas.context();

    Painter painter(cr);
    painter.set(font());

    size_t len = 0;
    float total = 0;
    for (detail::utf8_const_iterator ch(text.begin(), m_text.begin(), m_text.end());
         ch != detail::utf8_const_iterator(text.end(), m_text.begin(), m_text.end()); ++ch)
    {
        auto txt = detail::utf8_char_to_string(ch.base(), m_text.cend());
        cairo_text_extents_t te;
        cairo_text_extents(cr.get(), txt.c_str(), &te);
        if (total + te.x_advance > b.width())
        {
            return len;
        }
        total += te.x_advance;
        len++;
    }

    return len;
}

size_t TextBox::insert(const std::string& str)
{
    if (str.empty())
        return 0;

    auto current_len = detail::utf8len(m_text);
    auto len = detail::utf8len(str);

    if (m_max_len)
    {
        if (current_len + len > m_max_len)
            len = m_max_len - current_len;
    }

    if (!text_flags().is_set(TextFlag::multiline) &&
        text_flags().is_set(TextFlag::fit_to_width))
    {
        /*
         * Have to go through the motions and build the expected string at this
         * point to see what will fit.
         */

        // insert at cursor position
        auto text = m_text;
        auto i = text.begin();
        utf8::advance(i, m_cursor_pos, text.end());
        auto end = str.begin();
        utf8::advance(end, len, str.end());
        text.insert(i, str.begin(), end);

        auto maxlen = width_to_len(text);
        if (current_len + len > maxlen)
            len = maxlen - current_len;
    }

    if (m_validate_input)
        if (!validate_input(str))
            return 0;

    if (len)
    {
        // insert at cursor position
        auto i = m_text.begin();
        utf8::advance(i, m_cursor_pos, m_text.end());
        auto end = str.begin();
        utf8::advance(end, len, str.end());
        m_text.insert(i, str.begin(), end);
        cursor_forward(len);
        clear_selection();

        invoke_handlers(EventId::property_changed);
        damage();
    }

    return len;
}

size_t TextBox::cursor() const
{
    return m_cursor_pos;
}

void TextBox::cursor_begin()
{
    cursor_set(0);
}

void TextBox::cursor_end()
{
    // one past end
    cursor_set(detail::utf8len(m_text));
}

void TextBox::cursor_forward(size_t count)
{
    cursor_set(m_cursor_pos + count);
}

void TextBox::cursor_backward(size_t count)
{
    if (m_cursor_pos > count)
        cursor_set(m_cursor_pos - count);
    else
        cursor_set(0);
}

void TextBox::cursor_set(size_t pos)
{
    const auto len = detail::utf8len(m_text);
    if (pos > len)
        pos = len;

    if (m_cursor_pos != pos)
    {
        m_cursor_pos = pos;
        damage();
    }

    show_cursor();
}

void TextBox::selection_all()
{
    selection(0, detail::utf8len(m_text));
}

void TextBox::selection(size_t pos, size_t length)
{
    const auto len = detail::utf8len(m_text);
    if (pos > len)
        pos = len;

    auto i = m_text.begin();
    utf8::advance(i, pos, m_text.end());
    size_t d = utf8::distance(i, m_text.end());
    if (length > d)
        length = d;

    if (pos != m_select_start || length != m_select_len)
    {
        m_select_start = pos;
        m_select_len = length;
        damage();
    }
}

void TextBox::clear_selection()
{
    if (m_select_len)
    {
        m_select_len = 0;
        damage();
    }
}

std::string TextBox::selected_text() const
{
    if (m_select_len)
    {
        auto i = m_text.begin();
        utf8::advance(i, m_select_start, m_text.end());
        auto l = i;
        utf8::advance(l, m_select_len, m_text.end());
        return m_text.substr(std::distance(m_text.begin(), i),
                             std::distance(i, l));
    }

    return std::string();
}

void TextBox::delete_selection()
{
    if (m_select_len)
    {
        auto i = m_text.begin();
        utf8::advance(i, m_select_start, m_text.end());
        auto l = i;
        utf8::advance(l, m_select_len, m_text.end());
        size_t p = utf8::distance(m_text.begin(), i);

        m_text.erase(i, l);
        cursor_set(p);
        clear_selection();
        invoke_handlers(EventId::property_changed);
        damage();
    }
}

void TextBox::input_validation_enabled(bool enabled)
{
    m_validate_input = enabled;
}

void TextBox::add_validator_function(ValidatorCallback callback)
{
    m_validator_callbacks.emplace_back(std::move(callback));
}

bool TextBox::validate_input(const std::string& str)
{
    for (auto& callback : m_validator_callbacks)
    {
        auto valid = callback(str);
        if (!valid)
            return false;
    }

    return true;
}

void TextBox::cursor_timeout()
{
    m_cursor_state = !m_cursor_state;
    damage();
}

void TextBox::show_cursor()
{
    m_cursor_state = true;
    m_timer.start();
    damage();
}

void TextBox::hide_cursor()
{
    m_timer.cancel();
    m_cursor_state = false;
    damage();
}

Size TextBox::min_size_hint() const
{
    auto text = m_text;
    if (text.empty())
        text = "Hello World";

    auto s = text_size(text);
    s += Widget::min_size_hint() + Size(0, CURSOR_Y_OFFSET * 2.);
    return s;
}

template<>
std::map<TextBox::TextFlag, char const*> detail::EnumStrings<TextBox::TextFlag>::data =
{
    {TextBox::TextFlag::fit_to_width, "fit_to_width"},
    {TextBox::TextFlag::multiline, "multiline"},
    {TextBox::TextFlag::word_wrap, "word_wrap"},
    {TextBox::TextFlag::no_virt_keyboard, "no_virt_keyboard"},
};

}
}
