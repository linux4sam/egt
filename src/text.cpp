/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/utf8text.h"
#include "egt/canvas.h"
#include "egt/detail/alignment.h"
#include "egt/detail/layout.h"
#include "egt/detail/string.h"
#include "egt/frame.h"
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/text.h"
#include "egt/virtualkeyboard.h"

using namespace egt::detail;

namespace egt
{
inline namespace v1
{

const alignmask TextBox::default_align = alignmask::expand;

TextBox::TextBox(const std::string& text,
                 alignmask text_align,
                 const flags_type::flags& flags) noexcept
    : TextBox(text, {}, text_align, flags)
{}

TextBox::TextBox(const std::string& text,
                 const Rect& rect,
                 alignmask text_align,
                 const flags_type::flags& flags) noexcept
    : TextWidget(text, rect, text_align),
      m_timer(std::chrono::seconds(1)),
      m_text_flags(flags)
{
    set_name("TextBox" + std::to_string(m_widgetid));

    set_border(theme().default_border());
    set_boxtype(Theme::boxtype::blank_rounded);
    set_padding(5);

    m_timer.on_timeout(std::bind(&TextBox::cursor_timeout, this));

    cursor_set_end();
}

TextBox::TextBox(Frame& parent,
                 const std::string& text,
                 alignmask text_align,
                 const flags_type::flags& flags) noexcept
    : TextBox(text, Rect(), text_align, flags)
{}

TextBox::TextBox(Frame& parent,
                 const std::string& text,
                 const Rect& rect,
                 alignmask text_align,
                 const flags_type::flags& flags) noexcept
    : TextBox(text, rect, text_align, flags)
{
    parent.add(*this);
}

void TextBox::handle(Event& event)
{
    Widget::handle(event);

    switch (event.id())
    {
    case eventid::on_gain_focus:
        show_cursor();
        break;
    case eventid::on_lost_focus:
        hide_cursor();
        if (!text_flags().is_set(flag::no_virt_keyboard))
            if (popup_virtual_keyboard())
                popup_virtual_keyboard()->hide();
        break;
    case eventid::pointer_click:
        keyboard_focus(this);
        if (!text_flags().is_set(flag::no_virt_keyboard))
            if (popup_virtual_keyboard())
                popup_virtual_keyboard()->show();
        break;
    case eventid::keyboard_down:
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
            set_selection(cursor() - 1, 1);
            delete_selection();
        }
        break;
    }
    case EKEY_DELETE:
    {
        set_selection(cursor(), 1);
        delete_selection();
        break;
    }
    case EKEY_ENTER:
    {
        if (text_flags().is_set(flag::multiline))
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
        if (text_flags().is_set(flag::multiline))
        {
            // TODO
        }
        break;
    }
    case EKEY_DOWN:
    {
        if (text_flags().is_set(flag::multiline))
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
            painter.set_line_width(2);

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
                      justification::start,
                      color(Palette::ColorId::text).color(),
                      draw_cursor,
                      m_cursor_pos,
                      color(Palette::ColorId::text_highlight).color(),
                      m_select_start,
                      m_select_len);
}

void TextBox::set_text(const std::string& str)
{
    clear_selection();
    clear();
    insert(str);
}

void TextBox::clear()
{
    clear_selection();
    cursor_set_begin();
    TextWidget::clear();
}

void TextBox::set_max_length(size_t len)
{
    if (detail::change_if_diff<>(m_max_len, len))
    {
        if (m_max_len)
        {
            auto len = utf8len(m_text);
            if (len > m_max_len)
            {
                auto i = m_text.begin();
#ifdef UTF8CPP_CHECKED
                utf8ns::advance(i, m_max_len, m_text.end());
#else
                utf8ns::advance(i, m_max_len);
#endif
                m_text.erase(i, m_text.end());
            }
        }

        damage();
    }
}

size_t TextBox::append(const std::string& str)
{
    cursor_set_end();
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
#ifdef UTF8CPP_CHECKED
    for (utf8_const_iterator ch(text.begin(), m_text.begin(), m_text.end());
         ch != utf8_const_iterator(text.end(), m_text.begin(), m_text.end()); ++ch)
#else
    for (utf8_const_iterator ch(text.begin());
         ch != utf8_const_iterator(text.end()); ++ch)
#endif
    {
#ifdef UTF8CPP_CHECKED
        auto txt = utf8_char_to_string(ch.base(), m_text.cend());
#else
        auto txt = utf8_char_to_string(ch.base());
#endif
        cairo_text_extents_t te;
        cairo_text_extents(cr.get(), txt.c_str(), &te);
        if (total + te.x_advance > b.width)
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

    auto current_len = utf8len(m_text);
    auto len = utf8len(str);

    if (m_max_len)
    {
        if (current_len + len > m_max_len)
            len = m_max_len - current_len;
    }

    if (!text_flags().is_set(flag::multiline) &&
        text_flags().is_set(flag::fit_to_width))
    {
        /*
         * Have to go through the motions and build the expected string at this
         * point to see what will fit.
         */

        // insert at cursor position
        auto text = m_text;
        auto i = text.begin();
#ifdef UTF8CPP_CHECKED
        utf8ns::advance(i, m_cursor_pos, text.end());
#else
        utf8ns::advance(i, m_cursor_pos);
#endif
        auto end = str.begin();
#ifdef UTF8CPP_CHECKED
        utf8ns::advance(end, len, str.end());
#else
        utf8ns::advance(end, len);
#endif
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
#ifdef UTF8CPP_CHECKED
        utf8ns::advance(i, m_cursor_pos, m_text.end());
#else
        utf8ns::advance(i, m_cursor_pos);
#endif
        auto end = str.begin();
#ifdef UTF8CPP_CHECKED
        utf8ns::advance(end, len, str.end());
#else
        utf8ns::advance(end, len);
#endif
        m_text.insert(i, str.begin(), end);
        cursor_forward(len);
        clear_selection();

        invoke_handlers(eventid::property_changed);
        damage();
    }

    return len;
}

size_t TextBox::cursor() const
{
    return m_cursor_pos;
}

void TextBox::cursor_set_begin()
{
    cursor_set(0);
}

void TextBox::cursor_set_end()
{
    // one past end
    cursor_set(utf8len(m_text));
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
    if (pos > utf8len(m_text))
        pos = utf8len(m_text);

    if (m_cursor_pos != pos)
    {
        m_cursor_pos = pos;
        damage();
    }

    show_cursor();
}

void TextBox::set_selection_all()
{
    set_selection(0, utf8len(m_text));
}

void TextBox::set_selection(size_t pos, size_t length)
{
    if (pos > utf8len(m_text))
        pos = utf8len(m_text);

    auto i = m_text.begin();
#ifdef UTF8CPP_CHECKED
    utf8ns::advance(i, pos, m_text.end());
#else
    utf8ns::advance(i, pos);
#endif
    size_t d = utf8ns::distance(i, m_text.end());
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
#ifdef UTF8CPP_CHECKED
        utf8ns::advance(i, m_select_start, m_text.end());
#else
        utf8ns::advance(i, m_select_start);
#endif
        auto l = i;
#ifdef UTF8CPP_CHECKED
        utf8ns::advance(l, m_select_len, m_text.end());
#else
        utf8ns::advance(l, m_select_len);
#endif
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
#ifdef UTF8CPP_CHECKED
        utf8ns::advance(i, m_select_start, m_text.end());
#else
        utf8ns::advance(i, m_select_start);
#endif
        auto l = i;
#ifdef UTF8CPP_CHECKED
        utf8ns::advance(l, m_select_len, m_text.end());
#else
        utf8ns::advance(l, m_select_len);
#endif
        size_t p = utf8ns::distance(m_text.begin(), i);

        m_text.erase(i, l);
        cursor_set(p);
        clear_selection();
        invoke_handlers(eventid::property_changed);
        damage();
    }
}

void TextBox::set_input_validation_enabled(bool enabled)
{
    m_validate_input = enabled;
}

void TextBox::add_validator_function(validator_callback_t callback)
{
    m_validator_callbacks.push_back(callback);
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

}
}
