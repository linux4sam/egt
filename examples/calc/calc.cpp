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

static std::string last_line(const string& str)
{
    std::stringstream ss(str);
    std::string line;
    while (std::getline(ss, line, '\n'));
    return line;
}

int main(int argc, const char** argv)
{
    Application app(argc, argv, "calc");

    TopWindow win;

    StaticGrid topgrid(Rect(), 1, 2);
    topgrid.palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);
    win.add(expand(topgrid));

    TextBox text("");
    text.text_flags().set(TextBox::flag::multiline);
    topgrid.add(expand(text), 0, 0);
    text.set_text_align(alignmask::center | alignmask::right);
    text.set_font(Font(25));

    ImageLabel logo(Image("@microchip_logo_black.png"));
    logo.set_align(alignmask::left | alignmask::top, 10);
    win.add(logo);

    StaticGrid buttongrid(Rect(), 4, 5, 5);
    buttongrid.palette().set(Palette::ColorId::border, Palette::GroupId::normal, Palette::transparent);
    topgrid.add(expand(buttongrid), 0, 1);

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

            auto b = make_shared<Button>(label, Rect(Size(50, 50)));
            b->set_font(Font(30, Font::weightid::BOLD));

            b->on_event([&text, b](eventid event)
            {
                if (event != eventid::pointer_click)
                    return 0;

                if (b->text() == "=")
                {
                    string line = detail::replace_all(detail::replace_all(last_line(text.text()), "x", "*"), "÷", "/");
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

            buttongrid.add(expand(b), c, r);
        }
    }

    topgrid.reposition();

    win.show();

    return app.run();
}
