/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/text.h"
#include "egt/painter.h"

using namespace std;

namespace egt
{

    TextBox::TextBox(const std::string& str, const Rect& rect, alignmask align)
        : TextWidget(str, rect, align),
          m_timer(std::chrono::seconds(1))
    {
        m_timer.on_timeout(std::bind(&TextBox::cursor_timeout, this));

        palette().set(Palette::BG, Palette::GROUP_NORMAL, palette().color(Palette::LIGHT));
        palette().set(Palette::BG, Palette::GROUP_ACTIVE, palette().color(Palette::LIGHT));

        palette().set(Palette::BG, Palette::GROUP_DISABLED, palette().color(Palette::MID));
        palette().set(Palette::TEXT, Palette::GROUP_DISABLED, palette().color(Palette::DARK));
    }

    TextBox::TextBox(const Rect& rect, alignmask align)
        : TextBox(std::string(), rect, align)
    {}

    void TextBox::set_focus()
    {
        start_cursor();
    }

    void TextBox::lost_focus()
    {
        stop_cursor();
    }

    int TextBox::handle(eventid event)
    {
        switch (event)
        {
        case eventid::MOUSE_DOWN:
            set_focus();
            return 1;
        case eventid::KEYBOARD_DOWN:

            if (std::isprint((char)event_key()))
            {
                m_text.append(1, (char)event_key());
                damage();
            }
            else if (event_code() == KEY_BACKSPACE)
            {
                m_text.pop_back();
                damage();
            }

            return 1;
        default:
            break;
        }

        return Widget::handle(event);
    }

    void TextBox::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        // box
        painter.draw_box(*this, Painter::boxtype::rounded_borderfill);

        // text
        Rect bounding = painter.draw_text(m_text, box(),
                                          palette().color(Palette::TEXT),
                                          m_text_align,
                                          5,
                                          font());

        if (/*focus() &&*/ m_cursor_state)
        {
            painter.set_color(palette().color(Palette::BORDER));
            painter.set_line_width(2);

            auto YOFF = 2;
            painter.line(bounding.top_right() + Point(0, -YOFF),
                         bounding.bottom_right() + Point(0, YOFF));
            painter.stroke();
        }
    }

    void TextBox::append(const std::string& str)
    {
        if (!str.empty())
        {
            m_text += str;
            damage();
        }
    }

    void TextBox::cursor_timeout()
    {
        m_cursor_state = !m_cursor_state;
        damage();
    }

    void TextBox::start_cursor()
    {
        m_cursor_state = true;
        m_timer.start();
        damage();
    }

    void TextBox::stop_cursor()
    {
        m_timer.cancel();
        m_cursor_state = false;
        damage();
    }

    TextBox::~TextBox()
    {}

    MultilineTextBox::MultilineTextBox(const std::string& str,
                                       const Rect& rect,
                                       alignmask align)
        : TextBox(str, rect, align)
    {}

    MultilineTextBox::MultilineTextBox(const Rect& rect,
                                       alignmask align)
        : MultilineTextBox(std::string(), rect, align)
    {}

    void MultilineTextBox::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        // box
        painter.draw_box(*this, Painter::boxtype::rounded_borderfill);

        // text
        painter.set_font(font());

        auto cr = painter.context();
        cairo_text_extents_t textext;
        cairo_text_extents(cr.get(), "A", &textext);

        std::stringstream ss(m_text);
        std::string line;

        Rect linerect = box();
        linerect.y += textext.height / 3;
        linerect.h = textext.height + textext.height / 3;

        size_t n = std::count(m_text.begin(), m_text.end(), '\n');
        size_t lines = box().h / linerect.h;

        while (std::getline(ss, line, '\n'))
        {
            if (n-- >= lines)
                continue;

            painter.draw_text(line, linerect,
                              palette().color(Palette::TEXT),
                              m_text_align,
                              5,
                              font());
            linerect.y += linerect.h;
        }
    }

    std::string MultilineTextBox::last_line() const
    {
        std::stringstream ss(m_text);
        std::string line;
        while (std::getline(ss, line, '\n'));
        return line;
    }

    MultilineTextBox::~MultilineTextBox()
    {
    }

}
