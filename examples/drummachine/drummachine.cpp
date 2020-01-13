/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <iostream>
#include <vector>

using namespace std;
using namespace egt;
using namespace egt::experimental;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "drummachine");

    TopWindow win;
    win.color(Palette::ColorId::bg, Palette::black);

    VerticalBoxSizer sizer;
    win.add(expand(sizer));

    ImageLabel logo(Image("@128px/egt_logo_white.png"));
    logo.margin(10);
    sizer.add(top(center(logo)));

    StaticGrid grid(std::make_tuple(3, 3));
    sizer.add(expand(grid));

    vector<std::pair<std::string, std::string>> drums =
    {
        {"Closed-Hi-Hat-1.wav", "Hi-Hat"},
        {"Crash-Cymbal-1.wav", "Symbol"},
        {"Ensoniq-ESQ-1-Hi-Synth-Tom.wav", "Tom"},
        {"Alesis-Sanctuary-QCard-Loose-Bell-C5.wav", "Bell"},
        {"Ensoniq-ESQ-1-Snare.wav", "Snare"},
        {"Bass-Drum-1.wav", "Bass"},
    };

    for (auto& drum : drums)
    {
        auto button = make_shared<Button>(drum.second);
        button->color(Palette::ColorId::button_bg, Palette::purple);
        button->color(Palette::ColorId::button_bg, Palette::purple, Palette::GroupId::active);
        button->border(2);
        button->color(Palette::ColorId::border, Palette::black);
        button->color(Palette::ColorId::border, Palette::red, Palette::GroupId::active);
        button->resize(Size(100, 100));

        auto sound = make_shared<Sound>(drum.first);
        button->on_event([sound](Event&)
        {
            sound->play();
        }, {EventId::raw_pointer_down});

        grid.add(center(button));
    }

    win.show();

    return app.run();
}
