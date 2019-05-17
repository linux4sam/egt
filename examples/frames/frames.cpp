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
    win0.flags().set(Widget::flag::no_layout);

    auto a = alignmask::top | alignmask::center;

    WindowType win1(Size(400, 400));
    win1.set_color(Palette::ColorId::bg, Palette::red);
    win1.set_name("red");
    {
        auto label = make_shared<Label>("x,y", Rect(0, 0, 100, 50));
        win1.on_event([&win1, label](Event & event)
        {
            auto p = win1.display_to_local(event.pointer().point);
            label->set_text(std::to_string(p.x) + "," + std::to_string(p.y));
        }, {eventid::raw_pointer_move});
        label->set_align(a);
        win1.add(label);
    }
    win0.add(win1);
    win1.move(Point(50, 50));
    auto l1 = make_shared<Label>(win1.box().tostring(), Rect(0, 0, 100, 50));
    l1->set_align(alignmask::center | alignmask::bottom);
    win1.add(l1);

    WindowType win2(Size(300, 300));
    win2.set_color(Palette::ColorId::bg, Palette::blue);
    win2.set_name("blue");
    {
        auto label = make_shared<Label>("x,y", Rect(0, 0, 100, 50));
        win2.on_event([&win2, label](Event & event)
        {
            auto p = win2.display_to_local(event.pointer().point);
            label->set_text(std::to_string(p.x) + "," + std::to_string(p.y));
        }, {eventid::raw_pointer_move});
        label->set_align(a);
        win2.add(label);
    }
    win1.add(win2);
    win2.move(Point(50, 50));
    auto l2 = make_shared<Label>(win2.box().tostring(), Rect(0, 0, 100, 50));
    l2->set_align(alignmask::center | alignmask::bottom);
    win2.add(l2);

    WindowType win3(Size(200, 200));
    win3.set_color(Palette::ColorId::bg, Palette::green);
    auto l3 = make_shared<Label>("win3", Rect(0, 0, 50, 50));
    l3->set_align(a);
    win3.add(l3);
    win3.set_name("green");
    win2.add(win3);
    win3.move(Point(50, 50));

    WindowType win4(Size(100, 100));
    win4.set_color(Palette::ColorId::bg, Palette::purple);
    auto l4 = make_shared<Label>("win4", Rect(0, 0, 50, 50));
    l4->set_align(a);
    win4.add(l4);
    win4.set_name("purple");
    win3.add(win4);
    win4.move(Point(50, 50));

    win0.show();
    win1.show();
    win2.show();
    win3.show();
    win4.show();

    app.dump(cout);

    return app.run();
}
