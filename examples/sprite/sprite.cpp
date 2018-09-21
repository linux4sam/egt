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
#ifdef HAVE_TSLIB
#ifdef HAVE_LIBPLANES
    KMSScreen screen;
    InputTslib input0("/dev/input/touchscreen0");
#else
    FrameBuffer fb("/dev/fb0");
#endif
#else
    X11Screen screen(Size(800, 480));
#endif

    SimpleWindow win;
    win.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::WHITE);

    StaticGrid grid(0, 0, win.w(), win.h(), 2, 2, 10);

    HardwareSprite sprite1("walk.png", 75, 132, 8, 0, 0, screen.size().w / 2 - 75, screen.size().h / 2 - 132 / 2);
    SoftwareSprite sprite2("walk.png", 75, 132, 8, 0, 0, screen.size().w / 2, screen.size().h / 2 - 132 / 2);
    win.add(&sprite1);
    grid.add(&sprite1, 0, 1, StaticGrid::ALIGN_CENTER);
    win.add(&sprite2);
    grid.add(&sprite2, 1, 1, StaticGrid::ALIGN_CENTER);
    win.add(grid.add(new Label("Hardware", Point(100, 100)), 0, 0, StaticGrid::ALIGN_CENTER | StaticGrid::ALIGN_BOTTOM));
    win.add(grid.add(new Label("Software", Point(200, 200)), 1, 0, StaticGrid::ALIGN_CENTER | StaticGrid::ALIGN_BOTTOM));

    grid.reposition();
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

    return EventLoop::run();
}
