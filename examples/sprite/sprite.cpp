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
    Application app(argc, argv, "sprite");

    TopWindow win;
    win.set_background(Image("background.png"));

    ImageLabel logo(Image("@128px/microchip_logo_white.png"));
    logo.set_align(alignmask::left | alignmask::top);
    logo.set_margin(10);
    win.add(logo);

    StaticGrid grid(Rect(Size(win.width(), win.height() - 40)), std::make_tuple(2, 2));

    Sprite sprite1(Image("walk.png"), Size(75, 132), 8, Point(0, 0));
    grid.add(center(sprite1), 0, 1);

    Sprite sprite2(Image("walk.png"), Size(75, 132), 8, Point(0, 0));
    grid.add(center(sprite2), 1, 1);

    CheckBox hardware_checkbox("Hardware", Rect(Point(0, 0), Size(120, 40)));
    hardware_checkbox.set_color(Palette::ColorId::bg, Palette::transparent);
    grid.add(center(hardware_checkbox), 0, 0);
    hardware_checkbox.set_checked(true);

    CheckBox software_checkbox("Software", Rect(Point(0, 0), Size(120, 40)));
    software_checkbox.set_color(Palette::ColorId::bg, Palette::transparent);
    grid.add(center(software_checkbox), 1, 0);
    software_checkbox.set_checked(true);

    hardware_checkbox.on_event([&](Event & event)
    {
        if (hardware_checkbox.checked())
            sprite1.show();
        else
            sprite1.hide();
    }, {eventid::property_changed});

    software_checkbox.on_event([&](Event & event)
    {
        if (software_checkbox.checked())
            sprite2.show();
        else
            sprite2.hide();
    }, {eventid::property_changed});

    win.add(grid);

    sprite1.show();
    sprite2.show();

    Label label2("FPS: -",
                 Rect(Point(0, 40), Size(100, 40)),
                 alignmask::center);
    label2.set_color(Palette::ColorId::text, Palette::black);
    label2.set_color(Palette::ColorId::bg, Palette::transparent);

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

    Slider slider1(Rect(Point(win.height() - 40, 300), Size(win.width(), 40)), 10, 500);
    win.add(bottom(center(slider1)));
    slider1.set_value(DEFAULT_MS_INTERVAL);
    slider1.on_event([&](Event&)
    {
        animatetimer.change_duration(std::chrono::milliseconds(slider1.value()));
    }, {eventid::property_changed});

    win.show();

    Popup popup(Size(100, 80));
    popup.move(Point(win.width() - 100 - 10, 10));
    popup.set_color(Palette::ColorId::bg, Palette::fuchsia);
    popup.set_name("popup");

    Label label1("CPU: -",
                 Rect(Point(0, 0), Size(100, 40)),
                 alignmask::center);
    label1.set_color(Palette::ColorId::text, Palette::black);
    label1.set_color(Palette::ColorId::bg, Palette::transparent);

    popup.add(label1);
    popup.add(label2);

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

    win.add(popup);
    popup.show();

    return app.run();
}
