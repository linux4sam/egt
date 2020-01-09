/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <egt/detail/string.h>
#include <egt/utils.h>
#include <string>
#include <vector>
#include <sstream>

using namespace egt;
using namespace std;

static std::string last_line(const string& str)
{
    std::stringstream ss(str);
    std::string line;
    while (std::getline(ss, line, '\n'))
    {}
    return line;
}

int main(int argc, const char** argv)
{
    Application app(argc, argv, "calc");

    TopWindow win;
    win.set_color(Palette::ColorId::bg, Palette::black);

    VerticalBoxSizer vsizer;
    win.add(expand(vsizer));

    ImageLabel elogo(Image("@128px/egt_logo_white.png"));
    elogo.set_margin(10);
    vsizer.add(elogo);

    TextBox text("");
    text.text_flags().set(TextBox::flag::multiline);
    text.set_text_align(alignmask::center | alignmask::right);
    text.set_font(Font(25));
    text.set_color(Palette::ColorId::bg, Color(0x272727ff));
    text.set_color(Palette::ColorId::text, Palette::white);
    text.set_readonly(true);
    vsizer.add(expand(text));

    const vector<vector<string>> buttons =
    {
        {"(", ")", "%", "C"},
        {"7", "8", "9", "/"},
        {"4", "5", "6", "x"},
        {"1", "2", "3", "-"},
        {"0", ".", "=", "+"},
    };

    vector<vector<pair<Palette::ColorId, Color>>> colors =
    {
        {
            make_pair(Palette::ColorId::button_bg, Palette::gray),
            make_pair(Palette::ColorId::button_bg, Palette::gray),
            make_pair(Palette::ColorId::button_bg, Palette::gray),
            make_pair(Palette::ColorId::button_bg, Palette::orange)
        },

        {
            make_pair(Palette::ColorId::button_bg, Palette::lightgray),
            make_pair(Palette::ColorId::button_bg, Palette::lightgray),
            make_pair(Palette::ColorId::button_bg, Palette::lightgray),
            make_pair(Palette::ColorId::button_bg, Palette::orange)
        },

        {
            make_pair(Palette::ColorId::button_bg, Palette::lightgray),
            make_pair(Palette::ColorId::button_bg, Palette::lightgray),
            make_pair(Palette::ColorId::button_bg, Palette::lightgray),
            make_pair(Palette::ColorId::button_bg, Palette::orange)
        },

        {
            make_pair(Palette::ColorId::button_bg, Palette::lightgray),
            make_pair(Palette::ColorId::button_bg, Palette::lightgray),
            make_pair(Palette::ColorId::button_bg, Palette::lightgray),
            make_pair(Palette::ColorId::button_bg, Palette::orange)
        },

        {
            make_pair(Palette::ColorId::button_bg, Palette::lightgray),
            make_pair(Palette::ColorId::button_bg, Palette::lightgray),
            make_pair(Palette::ColorId::button_bg, Palette::blue),
            make_pair(Palette::ColorId::button_bg, Palette::orange)
        },
    };

    for (size_t r = 0; r < buttons.size(); r++)
    {
        auto line_sizer = make_shared<HorizontalBoxSizer>(justification::middle);
        vsizer.add(expand_horizontal(line_sizer));

        for (size_t c = 0; c < buttons[r].size(); c++)
        {
            string label = buttons[r][c];
            if (label.empty())
                continue;

            auto b = make_shared<Button>(label, Rect(Size(60, 60)));
            b->flags().set(Widget::flag::no_autoresize);
            b->set_border(1);
            b->set_font(Font(25, Font::weightid::bold));
            b->set_color(colors[r][c].first, colors[r][c].second);
            b->set_color(Palette::ColorId::border, Palette::gray);
            line_sizer->add(b);

            b->on_event([&text, b](Event&)
            {
                static bool do_clear = false;

                if (b->text() == "=")
                {
                    string line = detail::replace_all(detail::replace_all(last_line(text.text()), "x", "*"), "÷", "/");
                    if (!line.empty())
                    {
                        ostringstream ss;
                        ss << "\n= " <<
                           egt::experimental::lua_evaluate(line) <<
                           "\n";
                        text.append(ss.str());
                    }
                    do_clear = true;
                }
                else if (b->text() == "C")
                {
                    text.clear();
                }
                else
                {
                    if (do_clear)
                    {
                        text.clear();
                        do_clear = false;
                    }
                    text.append(b->text());
                }
            }, {eventid::pointer_click});
        }
    }

    win.show();

    return app.run();
}
