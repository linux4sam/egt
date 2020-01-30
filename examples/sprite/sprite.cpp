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
    win.background(Image("background.png"));

    ImageLabel logo(Image("icon:128px/microchip_logo_white.png"));
    logo.align(AlignFlag::left | AlignFlag::top);
    logo.margin(10);
    win.add(logo);

    StaticGrid grid(Rect(Size(win.width(), win.height() - 40)), std::make_tuple(2, 2));

    Sprite sprite1(Image("walk.png"), Size(75, 132), 8, Point(0, 0));
    grid.add(center(sprite1), 0, 1);

    Sprite sprite2(Image("walk.png"), Size(75, 132), 8, Point(0, 0));
    grid.add(center(sprite2), 1, 1);

    CheckBox hardware_checkbox("Hardware", Rect(Point(0, 0), Size(120, 40)));
    hardware_checkbox.color(Palette::ColorId::bg, Palette::transparent);
    grid.add(center(hardware_checkbox), 0, 0);
    hardware_checkbox.checked(true);

    CheckBox software_checkbox("Software", Rect(Point(0, 0), Size(120, 40)));
    software_checkbox.color(Palette::ColorId::bg, Palette::transparent);
    grid.add(center(software_checkbox), 1, 0);
    software_checkbox.checked(true);

    hardware_checkbox.on_checked_changed([&]()
    {
        if (hardware_checkbox.checked())
            sprite1.show();
        else
            sprite1.hide();
    });

    software_checkbox.on_checked_changed([&]()
    {
        if (software_checkbox.checked())
            sprite2.show();
        else
            sprite2.hide();
    });

    win.add(grid);

    sprite1.show();
    sprite2.show();

    Label label2("FPS: -",
                 Rect(Point(0, 40), Size(100, 40)),
                 AlignFlag::center);
    label2.color(Palette::ColorId::text, Palette::black);
    label2.color(Palette::ColorId::bg, Palette::transparent);

#define DEFAULT_MS_INTERVAL 100

    egt::experimental::Fps fps;
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
            label2.text(ss.str());
        }
        else
        {
            label2.text("FPS: -");
        }
    });
    animatetimer.start();

    Slider slider1(Rect(Point(win.height() - 40, 300), Size(win.width(), 40)), 10, 500);
    win.add(bottom(center(slider1)));
    slider1.value(DEFAULT_MS_INTERVAL);
    slider1.on_value_changed([&]()
    {
        animatetimer.change_duration(std::chrono::milliseconds(slider1.value()));
    });

    win.show();

    Popup popup(Size(100, 80));
    popup.move(Point(win.width() - 100 - 10, 10));
    popup.color(Palette::ColorId::bg, Palette::fuchsia);
    popup.name("popup");

    Label label1("CPU: -",
                 Rect(Point(0, 0), Size(100, 40)),
                 AlignFlag::center);
    label1.color(Palette::ColorId::text, Palette::black);
    label1.color(Palette::ColorId::bg, Palette::transparent);

    popup.add(label1);
    popup.add(label2);

    egt::experimental::CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &label1]()
    {
        tools.update();

        ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage(0)) << "%";
        label1.text(ss.str());
    });
    cputimer.start();

    win.add(popup);
    popup.show();

    return app.run();
}
