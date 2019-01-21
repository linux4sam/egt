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

int main(int argc, const char** argv)
{
    Application app(argc, argv, "frames");

    TopWindow win0;

    auto a = alignmask::LEFT | alignmask::CENTER;

    BasicWindow win1(Size(400, 400));
    win1.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::RED);
    win1.set_name("red");
    win1.add(new Label("win1", Rect(0, 0, 50, 50)))->set_align(a);
    win0.add(&win1);
    win1.move(Point(50, 50));

    BasicWindow win2(Size(300, 300));
    win2.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLUE);
    win2.set_name("blue");
    win2.add(new Label("win2", Rect(0, 0, 50, 50)))->set_align(a);
    win1.add(&win2);
    win2.move(Point(50, 50));

    BasicWindow win3(Size(200, 200));
    win3.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::GREEN);
    win3.add(new Label("win3", Rect(0, 0, 50, 50)))->set_align(a);
    win3.set_name("green");
    win2.add(&win3);
    win3.move(Point(50, 50));

    BasicWindow win4(Size(100, 100));
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
