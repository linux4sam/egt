/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <egt/ui>
#include <string>
#include <iostream>
#include <chrono>

using namespace std;
using namespace egt;

int main()
{
    const Color NAVY(Color::CSS("#001f3f"));
    const Color BLUE(Color::CSS("#0074D9"));
    const Color AQUA(Color::CSS("#7FDBFF"));
    const Color TEAL(Color::CSS("#39CCCC"));
    const Color OLIVE(Color::CSS("#3D9970"));
    const Color GREEN(Color::CSS("#2ECC40"));
    const Color LIME(Color::CSS("#01FF70"));
    const Color YELLOW(Color::CSS("#FFDC00"));
    const Color ORANGE(Color::CSS("#FF851B"));
    const Color RED(Color::CSS("#FF4136"));
    const Color MAROON(Color::CSS("#85144b"));
    const Color FUCHSIA(Color::CSS("#F012BE"));
    const Color PURPLE(Color::CSS("#B10DC9"));
    const Color BLACK(Color::CSS("#111111"));
    const Color GRAY(Color::CSS("#AAAAAA"));
    const Color SILVER(Color::CSS("#DDDDDD"));

    Application app;

    set_image_path("../share/egt/examples/sprite/");

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
