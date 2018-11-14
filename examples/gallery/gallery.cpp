/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <cmath>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <egt/ui>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

#define SHARED_PATH "../share/egt/examples/gallery/"

int main()
{
    Application app;

    std::vector<std::string> files = experimental::glob(SHARED_PATH "*trailer*.png");

    Window win;
    win.name("win");
    win.flag_set(widgetmask::NO_BORDER);
    win.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);

    Image logo(SHARED_PATH "logo.png");
    win.add(&logo);
    logo.align(alignmask::LEFT | alignmask::TOP, 10);

    auto grid_height = (win.size().h - logo.h()) / 2;

    ScrolledView view0(Rect(0, logo.h(), win.size().w, grid_height));
    view0.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    view0.name("view0");
    win.add(&view0);

    /** @todo This is not respecting parent coordinate for origin point. */
    StaticGrid grid0(Rect(0, logo.h(), files.size() * 150, grid_height), files.size(), 1, 0);
    grid0.name("grid0");
    view0.add(&grid0);

    for (auto& file : files)
    {
        grid0.add(new Image(file), alignmask::CENTER);
    }

    ScrolledView view1(Rect(0, logo.h() + grid_height + 1, win.size().w, grid_height));
    view1.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    view1.name("view1");
    win.add(&view1);

    StaticGrid grid1(Rect(0, logo.h() + grid_height + 1, files.size() * 150, grid_height),
                     files.size(), 1, 0);
    grid1.name("grid1");
    view1.add(&grid1);

    for (auto& file : files)
    {
        grid1.add(new Image(file), alignmask::CENTER);
    }

    Popup<Window> popup(Size(main_screen()->size().w / 2, main_screen()->size().h / 2));
    popup.name("popup");
    Button button("Hello World");
    popup.add(button);
    button.align(alignmask::CENTER);
    button.name("hw");

    ImageButton settings(SHARED_PATH "settings.png", "", Rect(), widgetmask::NO_BORDER);
    win.add(&settings);
    settings.flag_set(widgetmask::NO_BACKGROUND);
    settings.align(alignmask::RIGHT | alignmask::TOP, 10);
    settings.on_event([&popup](eventid event)
    {
        if (event == eventid::MOUSE_UP)
        {
            if (popup.visible())
                popup.hide();
            else
                popup.show(true);
        }
    });
    win.add(&popup);

    win.show();

    return app.run();
}
