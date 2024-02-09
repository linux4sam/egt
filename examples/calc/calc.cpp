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
    egt::Application app(argc, argv);

    egt::TopWindow win;
    win.color(egt::Palette::ColorId::bg, egt::Palette::black);

    const auto screen_size = app.screen()->size();
    const auto landscape = screen_size.width() >= screen_size.height();

    egt::VerticalBoxSizer vsizer;
    win.add(expand(vsizer));

    egt::ImageLabel elogo(egt::Image("icon:egt_logo_white.png;128"));
    elogo.margin(10);
    // The size needs to be set again since the margin has been modified.
    const auto m = elogo.moat();
    const auto elogo_size = elogo.image().size_orig() + egt::Size(2 * m, 2 * m);
    elogo.resize(elogo_size);
    vsizer.add(elogo);

    egt::TextBox text("", egt::TextBox::TextFlag::multiline);
    text.text_align(egt::AlignFlag::center_vertical | egt::AlignFlag::right);
    text.font(egt::Font(25));
    text.color(egt::Palette::ColorId::bg, egt::Color(0x272727ff));
    text.color(egt::Palette::ColorId::text, egt::Palette::white);
    text.readonly(true);
    vsizer.add(expand(text));

    egt::StaticGrid grid({4, 5});
    grid.align(egt::AlignFlag::bottom | egt::AlignFlag::center_horizontal);
    egt::DefaultDim width;
    if (landscape)
    {
        // Buttons size is 60x60 for a 800x480 screen.
        width = 60 * grid.n_col() * screen_size.width() / 800;
    }
    else
    {
        // The grid expands horizontally.
        width = screen_size.width();
    }
    // Buttons are squares.
    auto grid_size = egt::Size(width, grid.n_row() * width / grid.n_col());
    grid.resize(grid_size);
    vsizer.add(grid);

    const std::vector<std::vector<std::string>> buttons =
    {
        {"(", ")", "%", "C"},
        {"7", "8", "9", "/"},
        {"4", "5", "6", "x"},
        {"1", "2", "3", "-"},
        {"0", ".", "=", "+"},
    };

    const std::vector<std::vector<std::pair<egt::Palette::ColorId, egt::Color>>> colors =
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
        for (size_t c = 0; c < buttons[r].size(); c++)
        {
            std::string label = buttons[r][c];
            if (label.empty())
                continue;

            auto b = std::make_shared<egt::Button>(label);
            b->border(1);
            b->font(egt::Font(25, egt::Font::Weight::bold));
            b->color(colors[r][c].first, colors[r][c].second);
            b->color(egt::Palette::ColorId::border, egt::Palette::gray);
            grid.add(expand(b), c, r);

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
