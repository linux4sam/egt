/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <memory>

using namespace egt;
using namespace std;

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    TopWindow win;

    win.border(2);
    win.padding(10);
    win.margin(10);

    BoxSizer vsizer(win, orientation::vertical);
    expand(vsizer);

    BoxSizer sizer0(vsizer, orientation::horizontal);
    expand_horizontal(sizer0);
    sizer0.margin(10);

    for (auto i = 0; i < 4; ++i)
    {
        new Button(sizer0, "Button " + std::to_string(i));
    }

    BoxSizer sizer3(vsizer, orientation::horizontal);
    expand_horizontal(sizer3);

    for (auto i = 0; i < 4; ++i)
    {
        new Button(sizer3, "Button " + std::to_string(i));
    }

    BoxSizer sizer1(vsizer, orientation::vertical);
    expand(sizer1);

    for (auto i = 0; i < 4; ++i)
    {
        auto b = new Button(sizer1, "Button " + std::to_string(i));
        center(*b);
    }

    win.show();

    return app.run();
}
