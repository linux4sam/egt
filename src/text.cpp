/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/input.h"
#include "egt/painter.h"
#include "egt/text.h"

#include <iostream>
using namespace std;

namespace egt
{
inline namespace v1
{

TextBox::TextBox(const std::string& str, const Rect& rect, alignmask align,
                 const flags_type::flags& flags)
    : TextWidget(str, rect, align),
      m_timer(std::chrono::seconds(1)),
      m_text_flags(flags)
{
    set_name("TextBox" + std::to_string(m_widgetid));

    set_boxtype(Theme::boxtype::borderfill);

    m_timer.on_timeout(std::bind(&TextBox::cursor_timeout, this));

    cursor_set_end();
}

TextBox::TextBox(const Rect& rect, alignmask align, const flags_type::flags& flags)
    : TextBox(std::string(), rect, align, flags)
{}

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

void TextBox::draw(Painter& painter, const Rect& rect)
{
    ignoreparam(rect);

    auto group = disabled() ? Palette::GroupId::disabled : Palette::GroupId::normal;

    // box
    draw_box(painter);

    // text
    auto cr = painter.context();

    painter.set_font(font());

    cairo_font_extents_t fe;
    cairo_font_extents(cr.get(), &fe);

    using FontPoint = PointType<float, compatible::normal>;
    FontPoint offset(box().x, box().y);

    auto m_text_margin = 5.0;
    auto line_spacing = 0.5;

    offset.x += m_text_margin;
    offset.y += m_text_margin + line_spacing;

    auto draw_cursor = [&]()
    {
        if (focus() && m_cursor_state)
        {
            painter.set_color(palette().color(Palette::ColorId::border));
            painter.set_line_width(2);

            auto YOFF = 2.;
            painter.line(offset + FontPoint(0, -YOFF),
                         offset + FontPoint(0, fe.height) + FontPoint(0, YOFF));
            painter.stroke();
        }
    };

    auto word_width = [&](std::string::iterator i)
    {
        size_t width = 0;

        // first skip spaces
        while (i != m_text.end())
        {
            if (!std::isspace(*i))
                break;

            std::string text(1, *i);
            cairo_text_extents_t te;
            cairo_text_extents(cr.get(), text.c_str(), &te);
            width += te.x_advance;
            ++i;
        }

        while (i != m_text.end())
        {
            if (std::isspace(*i))
                break;

            std::string text(1, *i);
            cairo_text_extents_t te;
            cairo_text_extents(cr.get(), text.c_str(), &te);
            width += te.x_advance;
            ++i;
        }

        return width;
    };

    size_t pos = 0;
    for (auto ch = m_text.begin(); ch != m_text.end(); ++ch)
    {
        // draw cursor if before current character
        if (pos == m_cursor_pos)
        {
            if (focus() && m_cursor_state)
                draw_cursor();
        }

        if (*ch == '\n' && text_flags().is_set(flag::multiline))
        {
            // newline
            offset.x = box().x + m_text_margin;
            offset.y += line_spacing + fe.height;
        }
        else if (std::isprint(*ch))
        {
            // sent font color
            painter.set_color(palette().color(Palette::ColorId::text, group));

            if (text_flags().is_set(flag::multiline) &&
                text_flags().is_set(flag::word_wrap))
            {
                if (std::isspace(*ch))
                {
                    auto width = word_width(ch);

                    if ((offset.x + width) >= box().w - m_text_margin)
                    {
                        // newline
                        offset.x = box().x + m_text_margin;
                        offset.y += line_spacing + fe.height;
                    }
                }
            }

            std::string text(1, *ch);

            cairo_text_extents_t te;
            cairo_text_extents(cr.get(), text.c_str(), &te);

            cairo_move_to(cr.get(), offset.x - te.x_bearing,
                          offset.y - fe.descent + fe.height);
            cairo_show_text(cr.get(), text.c_str());

            offset.x += te.x_advance;
        }

        pos++;
    }

    // handle cursor after last character
    if (pos == m_cursor_pos)
    {
        if (focus() && m_cursor_state)
            draw_cursor();
    }
}

void TextBox::set_text(const std::string& str)
{
    clear_selection();
    TextWidget::set_text(str);
    cursor_set_end();
}

void TextBox::clear()
{
    clear_selection();
    cursor_set_begin();
    TextWidget::clear();
}

size_t TextBox::append(const std::string& str)
{
    cursor_set_end();
    return insert(str);
}

size_t TextBox::insert(const std::string& str)
{
    if (!str.empty())
    {
        m_text.insert(m_cursor_pos, str);
        cursor_forward(str.size());
        clear_selection();
        damage();
    }

    return str.size();
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
    cursor_set(m_text.size());
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
    if (pos > m_text.size())
        pos = m_text.size();

    if (m_cursor_pos != pos)
    {
        m_cursor_pos = pos;
        damage();
    }

    show_cursor();
}

void TextBox::set_selection_all()
{
    set_selection(0, m_text.size());
}

void TextBox::set_selection(size_t pos, size_t length)
{
    if (pos > m_text.size())
        pos = m_text.size();

    if (pos + length > m_text.size())
        length = pos + length - m_text.size();

    m_select_start = pos;
    m_select_len = length;
    damage();
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
        return m_text.substr(m_select_start, m_select_len);

    return std::string();
}

void TextBox::delete_selection()
{
    if (m_select_len)
    {
        m_text.erase(m_select_start, m_select_len);
        cursor_set(m_select_start);
        clear_selection();
        damage();
    }
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
