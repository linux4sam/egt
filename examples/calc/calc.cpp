/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <string>
#include <vector>
#include <sstream>

using namespace egt;
using namespace std;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "calc");

    TopWindow win;

    StaticGrid topgrid(Rect(), 1, 2);
    topgrid.palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);
    win.add(&topgrid);
    topgrid.set_align(alignmask::EXPAND);

    MultilineTextBox text("");
    topgrid.add(&text, 0, 0, alignmask::EXPAND);
    text.set_text_align(alignmask::CENTER | alignmask::RIGHT);
    text.set_font(Font(25));

    ImageLabel logo(Image("@microchip_logo_black.png"));
    win.add(&logo)->set_align(alignmask::LEFT | alignmask::TOP, 10);

    StaticGrid buttongrid(Rect(), 4, 5, 5);
    buttongrid.palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);
    topgrid.add(&buttongrid, 0, 1, alignmask::EXPAND);
    buttongrid.set_align(alignmask::EXPAND);

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

            b = new Button(label, Rect(Size(50, 50)));
            b->set_font(Font(30, Font::weightid::BOLD));

            b->on_event([&text, b](eventid event)
            {
                if (event != eventid::POINTER_CLICK)
                    return 0;

                if (b->text() == "=")
                {
                    string line = detail::replace_all(detail::replace_all(text.last_line(), "x", "*"), "÷", "/");
                    if (!line.empty())
                    {
                        ostringstream ss;
                        ss << "\n= " <<
                           experimental::lua_evaluate(line) <<
                           "\n";
                        text.append(ss.str());
                    }
                }
                else if (b->text() == "C")
                {
                    text.clear();
                }
                else
                {
                    text.append(b->text());
                }

                return 0;
            });

            buttongrid.add(b, c, r, alignmask::EXPAND);
        }
    }

    topgrid.reposition();

    win.show();

    return app.run();
}
