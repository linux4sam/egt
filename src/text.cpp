/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/text.h"
#include "mui/painter.h"

using namespace std;

namespace mui
{

    TextBox::TextBox(const std::string& text, const Point& point, const Size& size)
        : Widget(point, size),
          m_text(text)
    {}

    int TextBox::handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_DOWN:
            damage();
            focus(true);
            return 1;
        case EVT_KEY_DOWN:
            if (focus())
            {
                m_text.append(1, (char)key_value());
                damage();
                return 1;
            }
            break;
        }

        return Widget::handle(event);
    }

    void TextBox::draw(Painter& painter, const Rect& rect)
    {
        // box
        painter.draw_basic_box(box(),
                               palette().color(Palette::BORDER),
                               palette().color(Palette::TEXTBG));

        // text
        painter.draw_text(m_text, box(),
                          palette().color(Palette::TEXT),
                          m_text_align,
                          5,
                          m_font);
    }

    void TextBox::value(const std::string& text)
    {
        if (m_text != text)
        {
            m_text = text;
            damage();
        }
    }

    TextBox::~TextBox()
    {
    }

    MultilineTextBox::MultilineTextBox(const std::string& text, const Point& point, const Size& size)
        : TextBox(text, point, size)
    {}

    void MultilineTextBox::draw(Painter& painter, const Rect& rect)
    {
        // box
        painter.draw_basic_box(box(),
                               palette().color(Palette::BORDER),
                               palette().color(Palette::TEXTBG));

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

    MultilineTextBox::~MultilineTextBox()
    {
    }
}
