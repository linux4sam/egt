/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/alignment.h"
#include "egt/detail/layout.h"
#include "egt/detail/string.h"
#include "egt/input.h"
#include "egt/canvas.h"
#include "egt/painter.h"
#include "egt/text.h"
#include <iostream>
#include <utf8.h>

using namespace std;

static inline size_t utf8len(const std::string& str)
{
    return utf8::unchecked::distance(str.begin(), str.end());
}

namespace egt
{
inline namespace v1
{

TextBox::TextBox() noexcept
    : m_timer(std::chrono::seconds(1))
{
    set_name("TextBox" + std::to_string(m_widgetid));

    set_border(theme().default_border());
    set_boxtype(Theme::boxtype::blank_rounded);
    set_padding(5);
    set_text_align(alignmask::center | alignmask::left);

    m_timer.on_timeout(std::bind(&TextBox::cursor_timeout, this));

    cursor_set_end();
}

TextBox::TextBox(const std::string& str, alignmask align,
                 const flags_type::flags& flags) noexcept
    : TextBox(str, Rect(), align, flags)
{}

TextBox::TextBox(const std::string& str, const Rect& rect, alignmask align,
                 const flags_type::flags& flags) noexcept
    : TextWidget(str, rect, align),
      m_timer(std::chrono::seconds(1)),
      m_text_flags(flags)
{
    set_name("TextBox" + std::to_string(m_widgetid));

    set_border(theme().default_border());
    set_boxtype(Theme::boxtype::blank_rounded);
    set_padding(5);
    set_text_align(alignmask::center | alignmask::left);

    m_timer.on_timeout(std::bind(&TextBox::cursor_timeout, this));

    cursor_set_end();
}

TextBox::TextBox(const Rect& rect, alignmask align, const flags_type::flags& flags) noexcept
    : TextBox(std::string(), rect, align, flags)
{}

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
        break;
    case eventid::pointer_click:
        keyboard_focus(this);
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
            string tmp;
            utf8::append(key.unicode, std::back_inserter(tmp));
            insert(tmp);
        }
        break;
    }
    }
}

using utf8_const_iterator = utf8::unchecked::iterator<std::string::const_iterator>;
using utf8_iterator = utf8::unchecked::iterator<std::string::iterator>;

template<class T>
inline std::string utf8_char_to_string(T ch)
{
    auto ch2 = ch;
    utf8::unchecked::advance(ch2, 1);
    return std::string(ch.base(), ch2.base());
}

template<class T>
void tokenize_with_delimiters(T begin, T end,
                              T dbegin, T dend,
                              std::vector<std::string>& tokens)
{
    std::string token;

    for (auto pos = begin; pos != end; ++pos)
    {
        auto f = std::find(dbegin, dend, *pos);
        if (f != dend)
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }

            tokens.emplace_back(utf8_char_to_string(pos));
        }
        else
        {
            token.append(utf8_char_to_string(pos));
        }
    }

    if (!token.empty())
        tokens.push_back(token);
}

void TextBox::draw(Painter& painter, const Rect&)
{
    auto b = content_area();

    // box
    draw_box(painter, Palette::ColorId::bg, Palette::ColorId::border);

    auto cr = painter.context();

    painter.set(font());
    cairo_font_extents_t fe;
    cairo_font_extents(cr.get(), &fe);

    // function to draw the cursor
    auto draw_cursor = [&](const Point & offset)
    {
        if (focus() && m_cursor_state)
        {
            painter.set(color(Palette::ColorId::cursor).color());
            painter.set_line_width(2);

            auto YOFF = 2.;
            painter.draw(offset + Point(0, -YOFF),
                         offset + Point(0, fe.height) + Point(0, YOFF));
            painter.stroke();
        }
    };

    // tokenize based on words or codepoints
    const std::string delimiters = " \t\n\r";
    std::vector<std::string> tokens;
    if (text_flags().is_set(flag::multiline) && text_flags().is_set(flag::word_wrap))
    {
        tokenize_with_delimiters(utf8_const_iterator(m_text.begin()),
                                 utf8_const_iterator(m_text.end()),
                                 utf8_const_iterator(delimiters.begin()),
                                 utf8_const_iterator(delimiters.end()),
                                 tokens);
    }
    else
    {
        for (utf8_const_iterator ch(m_text.begin());
             ch != utf8_const_iterator(m_text.end()); ++ch)
        {
            tokens.emplace_back(utf8_char_to_string(ch));
        }
    }

    // setup rects for each word/codepoint
    std::vector<detail::LayoutRect> rects;
    int behave = 0;
    for (auto t = tokens.begin(); t != tokens.end(); ++t)
    {
        if (*t == "\n")
        {
            detail::LayoutRect r;
            r.behave = behave;
            r.rect = Rect(0, 0, 1, fe.height);
            rects.emplace_back(r);
        }
        else
        {
            cairo_text_extents_t te;
            cairo_text_extents(cr.get(), (*t).c_str(), &te);
            rects.emplace_back(detail::LayoutRect(behave, Rect(0, 0, te.x_advance, fe.height)));
        }

        if (*t == "\n")
            behave = 0x200;
        else
            behave = 0;
    }

    // perform the layout
    justification justify = justification::start;
    if ((text_align() & alignmask::left) == alignmask::left)
    {}
    else if ((text_align() & alignmask::right) == alignmask::right)
        justify = justification::end;
    else if ((text_align() & alignmask::center) == alignmask::center)
        justify = justification::middle;

    detail::flex_layout(b, rects, justify, orientation::flex);

    // draw the codepoints, cursor, and selected box
    size_t pos = 0;
    auto r = rects.begin();
    std::string last_char;
    bool workaround = false;
    bool stop = false;
    for (auto t = tokens.begin(); !stop && t != tokens.end(); ++t)
    {
        if (r->rect.y != 0 && !text_flags().is_set(flag::multiline))
            break;

        float roff = 0.;
        for (utf8_const_iterator ch((*t).begin()); ch != utf8_const_iterator((*t).end()); ++ch)
        {
            float char_width = 0;
            last_char = utf8_char_to_string(ch);

            if (*ch != '\n')
            {
                cairo_text_extents_t te;
                cairo_text_extents(cr.get(), last_char.c_str(), &te);
                char_width = te.x_advance;

                auto p = PointF(static_cast<float>(b.x) + static_cast<float>(r->rect.x) + roff + te.x_bearing,
                                static_cast<float>(b.y) + static_cast<float>(r->rect.y) + te.y_bearing - fe.descent + fe.height);

                if (workaround)
                    p.y += fe.height;

                auto s = SizeF(char_width, r->rect.h);

                if (pos >= m_select_start && pos < m_select_start + m_select_len)
                {
                    auto p2 = PointF(static_cast<float>(b.x) + static_cast<float>(r->rect.x) + roff,
                                     static_cast<float>(b.y) + static_cast<float>(r->rect.y));

                    if (workaround)
                        p2.y += fe.height;

                    auto rect = RectF(p2, s);
                    if (!rect.empty())
                    {
                        painter.set(color(Palette::ColorId::text_highlight).color());
                        painter.draw(rect);
                        painter.fill();
                    }
                }

                painter.set(color(Palette::ColorId::text).color());
                painter.draw(p);
                painter.draw(last_char);

                roff += char_width;
            }
            else
            {
                if (!text_flags().is_set(flag::multiline))
                    break;

                // if first char is a "\n" layout doesn't respond right
                if (last_char.empty())
                    workaround = true;
            }

            // draw cursor if before current character
            if (pos == m_cursor_pos)
            {
                if (focus() && m_cursor_state)
                {
                    auto p = Point(b.x + r->rect.x + roff - char_width, b.y + r->rect.y);
                    draw_cursor(p);
                }
            }

            pos++;
        }

        ++r;
    }

    // handle cursor after last character
    if (pos == m_cursor_pos)
    {
        if (focus() && m_cursor_state)
        {
            if (!rects.empty())
            {
                auto p = b.point() + rects.back().rect.point() + Point(rects.back().rect.w, 0);
                if (workaround)
                {
                    p.y += fe.height;
                }

                if (last_char == "\n")
                {
                    p.x = b.x;
                    p.y += fe.height;
                }

                draw_cursor(p);
            }
            else
                draw_cursor(b.point());
        }
    }
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
                utf8::unchecked::advance(i, m_max_len);
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

size_t TextBox::width_to_len(const string& text) const
{
    auto b = content_area();

    Canvas canvas(Size(100, 100));
    auto cr = canvas.context();

    Painter painter(cr);
    painter.set(font());

    size_t len = 0;
    float total = 0;
    for (utf8_const_iterator ch(text.begin());
         ch != utf8_const_iterator(text.end()); ++ch)
    {
        auto txt = utf8_char_to_string(ch);
        cairo_text_extents_t te;
        cairo_text_extents(cr.get(), txt.c_str(), &te);
        if (total + te.x_advance > b.w)
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
        utf8::unchecked::advance(i, m_cursor_pos);
        auto end = str.begin();
        utf8::unchecked::advance(end, len);
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
        utf8::unchecked::advance(i, m_cursor_pos);
        auto end = str.begin();
        utf8::unchecked::advance(end, len);
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
    utf8::unchecked::advance(i, pos);
    size_t d = utf8::unchecked::distance(i, m_text.end());
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

std::string TextBox::get_selected_text() const
{
    if (m_select_len)
    {
        auto i = m_text.begin();
        utf8::unchecked::advance(i, m_select_start);
        auto l = i;
        utf8::unchecked::advance(l, m_select_len);
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
        utf8::unchecked::advance(i, m_select_start);
        auto l = i;
        utf8::unchecked::advance(l, m_select_len);
        size_t p = utf8::unchecked::distance(m_text.begin(), i);

        m_text.erase(i, l);
        cursor_set(p);
        clear_selection();
        invoke_handlers(eventid::property_changed);
        damage();
    }
}

void TextBox::enable_input_validation()
{
    m_validate_input = true;
}

void TextBox::disable_input_validation()
{
    m_validate_input = true;
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

static const auto DEFAULT_TEXTBOX_SIZE = Size(100, 50);

/// @todo this is not an acceptable computation,
/// especially for multi-line text
Size TextBox::min_size_hint() const
{
    if (!m_text.empty())
    {
        auto s = text_size(m_text);
        s += Widget::min_size_hint();
        return s;
    }

    return DEFAULT_TEXTBOX_SIZE + Widget::min_size_hint();
}

}
}
