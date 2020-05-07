/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/detail/string.h>
#include <egt/ui>
#include <sstream>
#include <string>
#include <vector>

static std::string last_line(const std::string& str)
{
    std::stringstream ss(str);
    std::string line;
    while (std::getline(ss, line, '\n'))
    {}
    return line;
}

int main(int argc, char** argv)
{
    egt::Application app(argc, argv, "calc");

    egt::TopWindow win;
    win.color(egt::Palette::ColorId::bg, egt::Palette::black);

    egt::VerticalBoxSizer vsizer;
    win.add(expand(vsizer));

    egt::ImageLabel elogo(egt::Image("icon:egt_logo_white.png;128"));
    elogo.margin(10);
    vsizer.add(elogo);

    egt::TextBox text("", egt::TextBox::TextFlag::multiline);
    text.text_align(egt::AlignFlag::center | egt::AlignFlag::right);
    text.font(egt::Font(25));
    text.color(egt::Palette::ColorId::bg, egt::Color(0x272727ff));
    text.color(egt::Palette::ColorId::text, egt::Palette::white);
    text.readonly(true);
    vsizer.add(expand(text));

    const std::vector<std::vector<std::string>> buttons =
    {
        {"(", ")", "%", "C"},
        {"7", "8", "9", "/"},
        {"4", "5", "6", "x"},
        {"1", "2", "3", "-"},
        {"0", ".", "=", "+"},
    };

    std::vector<std::vector<std::pair<egt::Palette::ColorId, egt::Color>>> colors =
    {
        {
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::gray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::gray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::gray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::orange)
        },

        {
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::lightgray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::lightgray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::lightgray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::orange)
        },

        {
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::lightgray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::lightgray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::lightgray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::orange)
        },

        {
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::lightgray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::lightgray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::lightgray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::orange)
        },

        {
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::lightgray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::lightgray),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::blue),
            std::make_pair(egt::Palette::ColorId::button_bg, egt::Palette::orange)
        },
    };

    for (size_t r = 0; r < buttons.size(); r++)
    {
        auto line_sizer = std::make_shared<egt::HorizontalBoxSizer>(egt::Justification::middle);
        vsizer.add(expand_horizontal(line_sizer));

        for (size_t c = 0; c < buttons[r].size(); c++)
        {
            std::string label = buttons[r][c];
            if (label.empty())
                continue;

            auto b = std::make_shared<egt::Button>(label, egt::Rect(egt::Size(60, 60)));
            b->autoresize(false);
            b->border(1);
            b->font(egt::Font(25, egt::Font::Weight::bold));
            b->color(colors[r][c].first, colors[r][c].second);
            b->color(egt::Palette::ColorId::border, egt::Palette::gray);
            line_sizer->add(b);

            b->on_click([&text, b](egt::Event&)
            {
                static bool do_clear = false;

                if (b->text() == "=")
                {
                    auto line = egt::detail::replace_all(
                                    egt::detail::replace_all(
                                        last_line(text.text()), "x", "*"), "÷", "/");
                    if (!line.empty())
                    {
                        std::ostringstream ss;
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
            });
        }
    }

    win.show();

    return app.run();
}
