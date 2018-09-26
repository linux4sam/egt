/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "text.h"
#include "painter.h"

using namespace std;

namespace mui
{

    TextBox::TextBox(const std::string& text, const Point& point, const Size& size)
        : Widget(point.x, point.y, size.w, size.h),
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

    void TextBox::draw(const Rect& rect)
    {
        // box
        draw_basic_box(box(),
                       palette().color(Palette::BORDER),
                       palette().color(Palette::TEXTBG));

        // text
        draw_text(m_text, box(),
                  palette().color(Palette::TEXT),
                  ALIGN_CENTER | ALIGN_LEFT);
    }

    TextBox::~TextBox()
    {
    }

}
