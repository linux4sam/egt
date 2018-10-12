/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui>
#include <string>
#include <vector>

using namespace mui;
using namespace std;

int main()
{
    Application app;

    Window win;

    StaticGrid topgrid(Point(), Size(), 1, 2);
    win.add(&topgrid);
    topgrid.align(Widget::ALIGN_EXPAND);

    MultilineTextBox text("", Point(), Size());
    topgrid.add(&text, 0, 0, Widget::ALIGN_EXPAND);
    text.text_align(Widget::ALIGN_CENTER | Widget::ALIGN_RIGHT);
    text.font(Font(20));

    StaticGrid buttongrid(Point(), Size(), 4, 5, 5);
    topgrid.add(&buttongrid, 0, 1, Widget::ALIGN_EXPAND);
    buttongrid.align(Widget::ALIGN_EXPAND);

    vector<vector<string>> buttons =
    {
        {"(", ")", "%", "AC"},
        {"7", "8", "9", "÷"},
        {"4", "5", "6", "x"},
        {"1", "2", "3", "-"},
        {"0", ".", "=", "+"},
    };

    for (size_t r = 0; r < buttons.size(); r++)
    {
        for (size_t c = 0; c < buttons[r].size(); c++)
        {
            string label = buttons[r][c];
            if (label.empty())
                continue;

            Button* b;

            b = new Button(label, Point(), Size(50, 50));
            b->font(Font(30, Font::WEIGHT_BOLD));

            b->add_handler([&text, b](EventWidget * widget)
            {
                if (b->active())
                {
                    if (b->text() == "=")
                    {
                        text.value(text.value() + "\n= 0\n");
                    }
                    else if (b->text() == "AC")
                    {
                        text.value("");
                    }
                    else
                    {
                        text.value(text.value() + b->text());
                    }
                }
            });

            buttongrid.add(b, c, r, Widget::ALIGN_EXPAND);

            if (label == "=")
            {
                b->palette().set(Palette::LIGHT, Palette::GROUP_NORMAL, Color::BLUE);
            }
        }
    }

    topgrid.reposition();
    buttongrid.reposition();

    win.show();

    return app.run();
}
