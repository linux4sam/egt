/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;
using namespace egt;


using WindowType = Window;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "frames");

    TopWindow win0;

    auto a = alignmask::TOP | alignmask::CENTER;

    WindowType win1(Size(400, 400));
    win1.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::RED);
    win1.set_name("red");
    {
        auto label = new Label("x,y", Rect(0, 0, 100, 50));
        win1.on_event([&win1, label](eventid event)
        {
            ignoreparam(event);
            auto p = win1.from_screen(event_mouse());
            label->set_text(std::to_string(p.x) + "," + std::to_string(p.y));
            return 0;
        }, {eventid::MOUSE_MOVE});
        label->set_align(a);
        win1.add(label);
    }
    win0.add(&win1);
    win1.move(Point(50, 50));
    win1.add(new Label(win1.box().tostring(), Rect(0, 0, 100, 50)))->set_align(alignmask::CENTER | alignmask::BOTTOM);

    WindowType win2(Size(300, 300));
    win2.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLUE);
    win2.set_name("blue");
    {
        auto label = new Label("x,y", Rect(0, 0, 100, 50));
        win2.on_event([&win2, label](eventid event)
        {
            ignoreparam(event);
            auto p = win2.from_screen(event_mouse());
            label->set_text(std::to_string(p.x) + "," + std::to_string(p.y));
            return 0;
        }, {eventid::MOUSE_MOVE});
        label->set_align(a);
        win2.add(label);
    }
    win1.add(&win2);
    win2.move(Point(50, 50));
    win2.add(new Label(win2.box().tostring(), Rect(0, 0, 100, 50)))->set_align(alignmask::CENTER | alignmask::BOTTOM);

    WindowType win3(Size(200, 200));
    win3.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::GREEN);
    win3.add(new Label("win3", Rect(0, 0, 50, 50)))->set_align(a);
    win3.set_name("green");
    win2.add(&win3);
    win3.move(Point(50, 50));

    WindowType win4(Size(100, 100));
    win4.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::PURPLE);
    win4.add(new Label("win4", Rect(0, 0, 50, 50)))->set_align(a);
    win4.set_name("purple");
    win3.add(&win4);
    win4.move(Point(50, 50));

    win0.show();
    win1.show();
    win2.show();
    win3.show();
    win4.show();

    app.dump(cout);

    return app.run();
}
