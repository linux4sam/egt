/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <memory>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv);

    egt::TopWindow win;

    win.border(2);
    win.padding(10);
    win.margin(10);

    egt::BoxSizer vsizer(win, egt::Orientation::vertical);
    expand(vsizer);

    egt::BoxSizer sizer0(vsizer, egt::Orientation::horizontal);
    expand_horizontal(sizer0);
    sizer0.margin(10);

    for (auto i = 0; i < 4; ++i)
    {
        new egt::Button(sizer0, "Button " + std::to_string(i));
    }

    egt::BoxSizer sizer3(vsizer, egt::Orientation::horizontal);
    expand_horizontal(sizer3);

    for (auto i = 0; i < 4; ++i)
    {
        new egt::Button(sizer3, "Button " + std::to_string(i));
    }

    egt::BoxSizer sizer1(vsizer, egt::Orientation::vertical);
    expand(sizer1);

    for (auto i = 0; i < 4; ++i)
    {
        auto b = new egt::Button(sizer1, "Button " + std::to_string(i));
        center(*b);
    }

    win.show();

    return app.run();
}
