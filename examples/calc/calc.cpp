/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui>
#include <string>
#include <vector>
#include <sstream>

using namespace mui;
using namespace std;

static std::string replace_all(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

int main()
{
    Application app;

    Window win;

    StaticGrid topgrid(Point(), Size(), 1, 2);
    win.add(&topgrid);
    topgrid.align(alignmask::EXPAND);

    MultilineTextBox text("", Point(), Size());
    topgrid.add(&text, 0, 0, alignmask::EXPAND);
    text.text_align(alignmask::CENTER | alignmask::RIGHT);
    text.font(Font(25));

    StaticGrid buttongrid(Point(), Size(), 4, 5, 5);
    topgrid.add(&buttongrid, 0, 1, alignmask::EXPAND);
    buttongrid.align(alignmask::EXPAND);

    vector<vector<string>> buttons =
    {
        {"(", ")", "%", "C"},
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

            b->add_handler([&text, b](EventWidget * widget, int event)
            {
                ignoreparam(widget);

                if (event != EVT_MOUSE_UP)
                    return;

                if (b->text() == "=")
                {
                    string line = replace_all(replace_all(text.last_line(), "x", "*"), "÷", "/");
                    ostringstream ss;
                    ss << "\n= " <<
                       experimental::lua_evaluate(line) <<
                       "\n";
                    text.append(ss.str());
                }
                else if (b->text() == "C")
                {
                    text.clear();
                }
                else
                {
                    text.append(b->text());
                }
            });

            buttongrid.add(b, c, r, alignmask::EXPAND);

            if (label == "=")
            {
                b->palette().set(Palette::LIGHT, Palette::GROUP_NORMAL, Color::BLUE);
                b->palette().set(Palette::LIGHT, Palette::GROUP_ACTIVE, Color::RED);
            }
        }
    }

    win.show();

    return app.run();
}
