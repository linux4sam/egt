/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui>
#include <string>
#include <iostream>
#include <chrono>

using namespace std;
using namespace mui;

int main()
{
    Application app;

    set_image_path("../share/mui/examples/sprite/");

    Window win;
    win.palette().set(Palette::BG, Palette::GROUP_NORMAL, FUCHSIA);

    StaticGrid grid(Rect(Size(win.w(), win.h())), 2, 2, 10);

    HardwareSprite sprite1("walk.png", Size(75, 132), 8, Point(0, 0),
                           Point(main_screen()->size().w / 2 - 75,
                                 main_screen()->size().h / 2 - 132 / 2));
    grid.add(&sprite1, 0, 1, alignmask::CENTER);

    SoftwareSprite sprite2("walk.png", Size(75, 132), 8, Point(0, 0),
                           Point(main_screen()->size().w / 2,
                                 main_screen()->size().h / 2 - 132 / 2));
    grid.add(&sprite2, 1, 1, alignmask::CENTER);

    grid.add(new Label("Hardware", Rect(0, 0, 100, 50)), 0, 0,
             alignmask::CENTER | alignmask::BOTTOM);
    grid.add(new Label("Software", Rect(0, 0, 100, 50)), 1, 0,
             alignmask::CENTER | alignmask::BOTTOM);

    win.add(&grid);

    sprite1.show();
    sprite2.show();
    win.show();

    PeriodicTimer animatetimer(std::chrono::milliseconds(100));
    animatetimer.on_timeout([&sprite1, &sprite2]()
    {
        sprite1.advance();
        sprite2.advance();
    });
    animatetimer.start();

    return app.run();
}
