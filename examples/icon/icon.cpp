/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <egt/detail/filesystem.h>
#include <egt/detail/imagecache.h>
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

    auto NAV_WIDTH = 80;

    ImageButton left(win, Image("arrow_left.png"), "", Rect(0, 0, NAV_WIDTH, win.h()));
    left.set_color(Palette::ColorId::bg, Palette::black);
    left.set_text_align(alignmask::center);
    left.set_align(alignmask::left);

    ImageButton right(win, Image("arrow_right.png"), "", Rect(0, 0, NAV_WIDTH, win.h()));
    right.set_color(Palette::ColorId::bg, Palette::black);
    right.set_text_align(alignmask::center);
    right.set_align(alignmask::right);

    ScrolledView view0(Rect(NAV_WIDTH, 0, win.w() - (NAV_WIDTH * 2), win.h()));
    view0.set_color(Palette::ColorId::bg, Palette::black);
    view0.set_name("view0");
    win.add(view0);

    PropertyAnimator swipe(0, 0, std::chrono::milliseconds(1000), easing_quintic_easein);
    swipe.on_change(std::bind(&ScrolledView::set_hoffset, std::ref(view0), std::placeholders::_1));

    right.on_event([&](Event&)
    {
        swipe.starting(view0.offset().x);
        swipe.ending(view0.offset().x - view0.w());
        swipe.start();
    }, {eventid::pointer_click});

    left.on_event([&](Event&)
    {
        swipe.starting(view0.offset().x);
        swipe.ending(view0.offset().x + view0.w());
        swipe.start();
    }, {eventid::pointer_click});

    std::vector<std::string> files = detail::glob(detail::resolve_file_path("icons/") + "*.png");

    StaticGrid grid0(Rect(0, 0, files.size() / 6 * 160, win.h()), Tuple(files.size() / 6, 6));
    grid0.set_name("grid0");
    view0.add(grid0);

    for (auto& file : files)
    {
        std::string basename = file.substr(file.find_last_of("/\\") + 1);
        auto icon = make_shared<ImageButton>(Image(file), basename);
        icon->set_boxtype(Theme::boxtype::none);
        icon->set_color(Palette::ColorId::button_text, Palette::black);
        grid0.add(icon);
    }

    win.show();

    const Color FUCHSIA(Color::CSS("#F012BE"));

    Popup popup(Size(100, 40));
    popup.move(Point(win.w() - 100 - 10 - NAV_WIDTH, 10));
    popup.set_color(Palette::ColorId::bg, FUCHSIA);
    popup.set_name("popup");

    Label label1("CPU: -",
                 Rect(Point(0, 0), Size(100, 40)),
                 alignmask::center);
    label1.set_color(Palette::ColorId::text, Palette::black);
    label1.set_color(Palette::ColorId::bg, Palette::transparent);
    win.add(popup);
    popup.add(label1);

    experimental::CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &label1]()
    {
        tools.update();

        ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage(0)) << "%";
        label1.set_text(ss.str());
    });
    cputimer.start();

    popup.show();

    return app.run();
}
