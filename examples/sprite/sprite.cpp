/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace egt;

int main(int argc, const char** argv)
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

    Application app(argc, argv);

    set_image_path("../share/egt/examples/sprite/");

    TopWindow win;
    ImageLabel img(Image("background.png"));
    win.add(&img);
    if (img.h() != win.h())
    {
        double scale = (double)win.h() / (double)img.h();
        img.scale_image(scale);
    }

    ImageLabel logo(Image("@microchip_logo_white.png"));
    win.add(&logo)->set_align(alignmask::LEFT | alignmask::TOP, 10);

    StaticGrid grid(Rect(Size(win.w(), win.h() - 40)), 2, 2);

    HardwareSprite sprite1(Image("walk.png"), Size(75, 132), 8, Point(0, 0),
                           Point(main_screen()->size().w / 2 - 75,
                                 main_screen()->size().h / 2 - 132 / 2));
    grid.add(&sprite1, 0, 1, alignmask::CENTER);

    SoftwareSprite sprite2(Image("walk.png"), Size(75, 132), 8, Point(0, 0),
                           Point(main_screen()->size().w / 2,
                                 main_screen()->size().h / 2 - 132 / 2));
    grid.add(&sprite2, 1, 1, alignmask::CENTER);

    CheckBox hardware_checkbox("Hardware", Rect(Point(0, 0), Size(120, 40)));
    grid.add(&hardware_checkbox, 0, 0, alignmask::CENTER);
    hardware_checkbox.check(true);

    CheckBox software_checkbox("Software", Rect(Point(0, 0), Size(120, 40)));
    grid.add(&software_checkbox, 1, 0, alignmask::CENTER);
    software_checkbox.check(true);

    hardware_checkbox.on_event([&](eventid event)
    {
        if (event == eventid::PROPERTY_CHANGED)
        {
            if (hardware_checkbox.checked())
                sprite1.show();
            else
                sprite1.hide();
        }
        return 0;
    });

    software_checkbox.on_event([&](eventid event)
    {
        if (event == eventid::PROPERTY_CHANGED)
        {
            if (software_checkbox.checked())
                sprite2.show();
            else
                sprite2.hide();
        }
        return 0;
    });


    win.add(&grid);

    sprite1.show();
    sprite2.show();

    Label label2("FPS: -",
                 Rect(Point(0, 40), Size(100, 40)),
                 alignmask::CENTER);
    label2.palette()
    .set(Palette::TEXT, Palette::GROUP_NORMAL, Color::BLACK)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

#define DEFAULT_MS_INTERVAL 100

    experimental::Fps fps;
    fps.start();

    PeriodicTimer animatetimer(std::chrono::milliseconds(DEFAULT_MS_INTERVAL));
    animatetimer.on_timeout([&]()
    {
        if (sprite1.visible())
            sprite1.advance();

        if (sprite2.visible())
            sprite2.advance();

        bool visible = sprite1.visible() || sprite2.visible();
        if (visible)
        {
            fps.end_frame();

            ostringstream ss;
            ss << "FPS: " << std::round(fps.fps());
            label2.set_text(ss.str());
        }
        else
        {
            label2.set_text("FPS: -");
        }
    });
    animatetimer.start();

    Slider slider1(Rect(Point(win.h() - 40, 300), Size(win.w(), 40)), 10, 500, 10);
    win.add(&slider1);
    slider1.set_align(alignmask::CENTER | alignmask::BOTTOM);
    slider1.set_value(DEFAULT_MS_INTERVAL);
    slider1.on_event([&](eventid event)
    {
        if (event == eventid::PROPERTY_CHANGED)
            animatetimer.change_duration(std::chrono::milliseconds(slider1.value()));
        return 0;
    });

    win.show();

    Popup<Window> popup(Size(100, 80));
    popup.move(Point(win.w() - 100 - 10, 10));
    popup.palette().set(Palette::BG, Palette::GROUP_NORMAL, FUCHSIA);
    popup.set_name("popup");

    Label label1("CPU: -",
                 Rect(Point(0, 0), Size(100, 40)),
                 alignmask::CENTER);
    label1.palette()
    .set(Palette::TEXT, Palette::GROUP_NORMAL, Color::BLACK)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

    popup.add(&label1);
    popup.add(&label2);

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
