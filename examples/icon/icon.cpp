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
    Application app(argc, argv, "icon");

    TopWindow win;
    win.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::WHITE);

    auto NAV_WIDTH = 80;

    ImageButton left(win, Image("arrow_left.png"), "", Rect(0, 0, NAV_WIDTH, win.h()), widgetmask::NONE);
    left.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    left.set_text_align(alignmask::CENTER);
    left.set_align(alignmask::LEFT);

    ImageButton right(win, Image("arrow_right.png"), "", Rect(0, 0, NAV_WIDTH, win.h()), widgetmask::NONE);
    right.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    right.set_text_align(alignmask::CENTER);
    right.set_align(alignmask::RIGHT);

    ScrolledView view0(Rect(NAV_WIDTH, 0, win.w() - (NAV_WIDTH * 2), win.h()));
    view0.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    view0.set_name("view0");
    win.add(&view0);

    PropertyAnimator swipe(0, 0, std::chrono::milliseconds(1000), easing_quintic_easein);
    swipe.on_change(std::bind(&ScrolledView::set_offset, std::ref(view0), std::placeholders::_1));

    right.on_event([&](eventid event)
    {
        if (event == eventid::POINTER_CLICK)
        {
            swipe.starting(view0.offset());
            swipe.ending(view0.offset() - view0.w());
            swipe.start();
        }
        return 0;
    });

    left.on_event([&](eventid event)
    {
        if (event == eventid::POINTER_CLICK)
        {
            swipe.starting(view0.offset());
            swipe.ending(view0.offset() + view0.w());
            swipe.start();
        }
        return 0;
    });

    std::vector<std::string> files = experimental::glob("../share/egt/icons/*.png");

    StaticGrid grid0(Rect(0, 0, files.size() / 6 * 160, win.h()), files.size() / 6, 6, 0);
    grid0.set_name("grid0");
    view0.add(&grid0);

    for (auto& file : files)
    {
        std::string basename = file.substr(file.find_last_of("/\\") + 1);
        auto icon = new ImageButton(Image(file), basename);
        icon->flag_set(widgetmask::NO_BACKGROUND);
        icon->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::BLACK);
        grid0.add(icon);
    }

    win.show();

    const Color FUCHSIA(Color::CSS("#F012BE"));

    Popup popup(Size(100, 40));
    popup.move(Point(win.w() - 100 - 10 - NAV_WIDTH, 10));
    popup.palette().set(Palette::BG, Palette::GROUP_NORMAL, FUCHSIA);
    popup.set_name("popup");

    Label label1("CPU: -",
                 Rect(Point(0, 0), Size(100, 40)),
                 alignmask::CENTER);
    label1.palette()
    .set(Palette::TEXT, Palette::GROUP_NORMAL, Color::BLACK)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
    win.add(&popup);
    popup.add(&label1);

    CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &label1]()
    {
        tools.update();

        ostringstream ss;
        ss << "CPU: " << (int)tools.usage(0) << "%";
        label1.set_text(ss.str());
    });
    cputimer.start();

    popup.show();

    return app.run();
}
