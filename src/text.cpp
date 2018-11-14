/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "egt/text.h"
#include "egt/painter.h"

using namespace std;

namespace egt
{

    TextBox::TextBox(const std::string& str, const Rect& rect,
                     alignmask align)
        : TextWidget(str, rect, align),
          m_timer(std::chrono::seconds(1))
    {
        m_timer.on_timeout(std::bind(&TextBox::cursor_timeout, this));
    }

    void TextBox::focus(bool value)
    {
        Widget::focus(value);

        if (value)
            start_cursor();
        else
            stop_cursor();
    }

    int TextBox::handle(eventid event)
    {
        switch (event)
        {
        case eventid::MOUSE_DOWN:
            return 1;
        case eventid::KEYBOARD_DOWN:

            if (std::isalnum((char)event_key()))
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
        painter.draw_box(box(),
                         palette().color(Palette::BORDER),
                         palette().color(Palette::TEXTBG),
                         Painter::boxtype::rounded_border);

        // text
        Rect bounding = painter.draw_text(m_text, box(),
                                          palette().color(Palette::TEXT),
                                          m_text_align,
                                          5,
                                          m_font);

        if (Widget::focus() && m_cursor_state)
        {
            auto cr = painter.context();

            cairo_text_extents_t textext;
            cairo_text_extents(cr.get(), m_text.c_str(), &textext);

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
        m_cursor_state = false;
        m_timer.cancel();
        damage();
    }

    TextBox::~TextBox()
    {}

    MultilineTextBox::MultilineTextBox(const std::string& str,
                                       const Rect& rect)
        : TextBox(str, rect)
    {}

    void MultilineTextBox::draw(Painter& painter, const Rect& rect)
    {
        ignoreparam(rect);

        // box
        painter.draw_box(box(),
                         palette().color(Palette::BORDER),
                         palette().color(Palette::TEXTBG),
                         Painter::boxtype::rounded_border);

        // text
        painter.set_font(m_font);

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
                              m_font);
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
