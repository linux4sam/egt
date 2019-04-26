/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/detail/alignment.h"
#include "egt/detail/layout.h"
#include "egt/input.h"
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

    set_boxtype(Theme::boxtype::borderfill);

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

    set_boxtype(Theme::boxtype::borderfill);

    m_timer.on_timeout(std::bind(&TextBox::cursor_timeout, this));

    cursor_set_end();
}

TextBox::TextBox(const Rect& rect, alignmask align, const flags_type::flags& flags) noexcept
    : TextBox(std::string(), rect, align, flags)
{}

TextBox::TextBox(const TextBox& rhs) noexcept
    : TextWidget(rhs),
      m_timer(std::chrono::seconds(1)),
      m_cursor_pos(rhs.m_cursor_pos),
      m_select_start(rhs.m_select_start),
      m_select_len(rhs.m_select_len),
      m_cursor_state(rhs.m_cursor_state),
      m_text_flags(rhs.m_text_flags),
      m_max_len(rhs.m_max_len)
{
    m_timer.on_timeout(std::bind(&TextBox::cursor_timeout, this));
}

TextBox& TextBox::operator=(const TextBox& rhs) noexcept
{
    if (&rhs != this)
    {
        TextWidget::operator=(rhs);

        m_cursor_pos = rhs.m_cursor_pos;
        m_select_start = rhs.m_select_start;
        m_select_len = rhs.m_select_len;
        m_cursor_state = rhs.m_cursor_state;
        m_text_flags = rhs.m_text_flags;
        m_max_len = rhs.m_max_len;

        m_timer.on_timeout(std::bind(&TextBox::cursor_timeout, this));
    }

    return *this;
}

int TextBox::handle(eventid event)
{
    switch (event)
    {
    case eventid::on_gain_focus:
        show_cursor();
        break;
    case eventid::on_lost_focus:
        hide_cursor();
        break;
    case eventid::pointer_click:
        keyboard_focus(this);
        return 1;
    case eventid::keyboard_down:
        return handle_key(event);
    default:
        break;
    }

    return Widget::handle(event);
}

int TextBox::handle_key(eventid event)
{
    ignoreparam(event);

    switch (event::keys().code)
    {
    case EKEY_BACK:
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
    case EKEY_RETURN:
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
        if (event::keys().key)
        {
            std::string str(1, event::keys().key);
            insert(str);
        }
        break;
    }
    }

    return 0;
}

using utf8_const_iterator = utf8::unchecked::iterator<std::string::const_iterator>;
using utf8_iterator = utf8::unchecked::iterator<std::string::iterator>;

void TextBox::draw(Painter& painter, const Rect&)
{
    auto b = box();
    b.shrink_around_center(5);

    auto group = Palette::GroupId::normal;
    if (disabled())
        group = Palette::GroupId::disabled;
    else if (active())
        group = Palette::GroupId::active;

    // box
    draw_box(painter);

    auto cr = painter.context();

    painter.set(font());
    cairo_font_extents_t fe;
    cairo_font_extents(cr.get(), &fe);

    auto draw_cursor = [&](const Point & offset)
    {
        if (focus() && m_cursor_state)
        {
            painter.set(Palette::red);
            painter.set_line_width(2);

            auto YOFF = 2.;
            painter.draw(offset + Point(0, -YOFF),
                         offset + Point(0, fe.height) + Point(0, YOFF));
            painter.stroke();
        }
    };

    vector<detail::LayoutRect> rects;

    uint32_t behave = 0;
    for (utf8_const_iterator ch(m_text.begin()); ch != utf8_const_iterator(m_text.end()); ++ch)
    {
        if (*ch == '\n')
        {
            detail::LayoutRect r;
            r.behave = behave;
            r.rect = Rect(0, 0, 1, fe.height);
            rects.emplace_back(r);
        }
        else
        {
            auto ch2 = ch;
            utf8::unchecked::advance(ch2, 1);
            std::string text(ch.base(), ch2.base());

            cairo_text_extents_t te;
            cairo_text_extents(cr.get(), (char*)text.c_str(), &te);
            detail::LayoutRect r;
            r.behave = behave;
            r.rect = Rect(0, 0, te.x_advance, fe.height);
            rects.emplace_back(r);
        }

        behave = 0;

        // put next character on newline
        if (*ch == '\n')
            behave |= 0x200;
    }

    justification justify = justification::start;
    if ((text_align() & alignmask::left) == alignmask::left)
    {}
    else if ((text_align() & alignmask::right) == alignmask::right)
        justify = justification::end;
    else if ((text_align() & alignmask::center) == alignmask::center)
        justify = justification::middle;

    detail::flex_layout(b, rects, justify, orientation::flex);

    size_t pos = 0;
    auto r = rects.begin();
    for (utf8_const_iterator ch(m_text.begin()); ch != utf8_const_iterator(m_text.end()); ++ch)
    {
        if (*ch != '\n')
        {
            auto ch2 = ch;
            utf8::unchecked::advance(ch2, 1);
            std::string text(ch.base(), ch2.base());
            cairo_text_extents_t te;
            cairo_text_extents(cr.get(), (char*)text.c_str(), &te);

            auto p = PointF((float)b.x + (float)r->rect.x + te.x_bearing,
                            (float)b.y + (float)r->rect.y + te.y_bearing - fe.descent + fe.height);


            auto s = SizeF(r->rect.w, r->rect.h);

            if (pos >= m_select_start && pos < m_select_start + m_select_len)
            {
                auto p2 = PointF((float)b.x + (float)r->rect.x,
                                 (float)b.y + (float)r->rect.y);

                auto rect = RectF(p2, s);
                if (!rect.empty())
                {
                    painter.set(Palette::aqua);
                    painter.draw(rect);
                    painter.fill();
                }
            }

            painter.set(palette().color(Palette::ColorId::text, group));
            painter.draw(p);
            painter.draw((char*)text.c_str());
        }
        else
        {
            if (!text_flags().is_set(flag::multiline))
                break;
        }

        // draw cursor if before current character
        if (pos == m_cursor_pos)
        {
            if (focus() && m_cursor_state)
                draw_cursor(Point(b.x + r->rect.x, b.y + r->rect.y));
        }

        ++r;
        pos++;
    }

    // handle cursor after last character
    if (pos == m_cursor_pos)
    {
        if (focus() && m_cursor_state)
        {
            if (!rects.empty())
            {
                auto p = b.point() + rects.back().rect.point() + Point(rects.back().rect.w, 0);
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
        /// @todo need to truncate m_text is max len got shorter
        damage();
    }
}

size_t TextBox::append(const std::string& str)
{
    cursor_set_end();
    return insert(str);
}

size_t TextBox::insert(const std::string& str)
{
    if (str.empty())
        return 0;

    auto len = utf8len(str);

    if (m_max_len)
    {
        auto current_len = utf8len(m_text);
        if (current_len + len > m_max_len)
            len = m_max_len - current_len;
    }

    if (m_validate_input)
        if (!validate_input(str))
            return 0;

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
    for (auto callback : m_validator_callbacks)
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

}
}
