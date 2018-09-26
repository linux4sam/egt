/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <mui/ui.h>
#include <string>
#include <iostream>
#include <chrono>

using namespace std;
using namespace mui;

int main()
{
    Application app;

    set_image_path("/root/mui/share/mui/examples/sprite/");

    Window win;
    win.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::WHITE);

    StaticGrid grid(Point(0, 0), Size(win.w(), win.h()), 2, 2, 10);

    HardwareSprite sprite1("walk.png", 75, 132, 8, 0, 0,
                           main_screen()->size().w / 2 - 75,
                           main_screen()->size().h / 2 - 132 / 2);
    grid.add(&sprite1, 0, 1, StaticGrid::ALIGN_CENTER);

    SoftwareSprite sprite2("walk.png", 75, 132, 8, 0, 0,
                           main_screen()->size().w / 2,
                           main_screen()->size().h / 2 - 132 / 2);
    grid.add(&sprite2, 1, 1, StaticGrid::ALIGN_CENTER);

    grid.add(new Label("Hardware", Point(100, 100)), 0, 0,
             StaticGrid::ALIGN_CENTER | StaticGrid::ALIGN_BOTTOM);
    grid.add(new Label("Software", Point(200, 200)), 1, 0,
             StaticGrid::ALIGN_CENTER | StaticGrid::ALIGN_BOTTOM);

    grid.reposition();
    win.add(&grid);

    sprite1.show();
    sprite2.show();
    win.show();

    PeriodicTimer animatetimer(100);
    animatetimer.add_handler([&sprite1, &sprite2]()
    {
        sprite1.advance();
        sprite2.advance();
    });
    animatetimer.start();

    return app.run();
}
