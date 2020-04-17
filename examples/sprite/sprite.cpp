/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <chrono>
#include <egt/ui>
#include <iomanip>
#include <sstream>

int main(int argc, char** argv)
{
    egt::Application app(argc, argv, "sprite");
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif

    egt::TopWindow win;
    win.background(egt::Image("file:background.png"));

    egt::ImageLabel logo(egt::Image("icon:microchip_logo_white.png;128"));
    logo.align(egt::AlignFlag::left | egt::AlignFlag::top);
    logo.margin(10);
    win.add(logo);

    egt::StaticGrid grid(egt::Rect(egt::Size(win.width(), win.height() - 40)),
                         egt::StaticGrid::GridSize(2, 2));

    egt::Sprite sprite1(egt::Image("file:needle.png"),
                        egt::Size(100, 100), 8 * 9, egt::Point(0, 0));
    grid.add(center(sprite1), 0, 1);

    // force software window
    egt::Sprite sprite2(egt::Image("file:walk.png"),
                        egt::Size(75, 132), 8, egt::Point(0, 0),
                        egt::WindowHint::software);
    grid.add(center(sprite2), 1, 1);

    egt::CheckBox hardware_checkbox("Hardware", egt::Rect(egt::Point(0, 0),
                                    egt::Size(120, 40)));
    hardware_checkbox.color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    grid.add(center(hardware_checkbox), 0, 0);
    hardware_checkbox.checked(true);

    egt::CheckBox software_checkbox("Software", egt::Rect(egt::Point(0, 0),
                                    egt::Size(120, 40)));
    software_checkbox.color(egt::Palette::ColorId::bg, egt::Palette::transparent);
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

    egt::Label label2("FPS: -",
                      egt::Rect(egt::Point(0, 40), egt::Size(100, 40)),
                      egt::AlignFlag::center);
    label2.color(egt::Palette::ColorId::text, egt::Palette::black);
    label2.color(egt::Palette::ColorId::bg, egt::Palette::transparent);

#define DEFAULT_MS_INTERVAL 100

    egt::experimental::FramesPerSecond fps;
    fps.start();

    egt::PeriodicTimer animatetimer(std::chrono::milliseconds(DEFAULT_MS_INTERVAL));
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

            std::ostringstream ss;
            ss << "FPS: " << std::round(fps.fps());
            label2.text(ss.str());
        }
        else
        {
            label2.text("FPS: -");
        }
    });
    animatetimer.start();

    egt::Slider slider(egt::Rect(egt::Point(win.height() - 80, 300),
                                 egt::Size(win.width(), 80)),
                       1, 100);
    slider.slider_flags().set(egt::Slider::SliderFlag::show_label);
    win.add(egt::bottom(egt::center(slider)));
    slider.on_value_changed([&]()
    {
        auto v = egt::detail::normalize<float>(slider.value(),
                                               slider.starting(),
                                               slider.ending(),
                                               10, 300);
        animatetimer.change_duration(std::chrono::milliseconds(static_cast<int>(v)));
    });

    slider.value(egt::detail::normalize<float>(DEFAULT_MS_INTERVAL,
                 10, 300,
                 slider.starting(),
                 slider.ending()));
    win.show();

    egt::Popup popup(egt::Size(100, 80));
    popup.move(egt::Point(win.width() - 100 - 10, 10));
    popup.color(egt::Palette::ColorId::bg, egt::Palette::fuchsia);

    egt::Label label1("CPU: -",
                      egt::Rect(egt::Point(0, 0), egt::Size(100, 40)),
                      egt::AlignFlag::center);
    label1.color(egt::Palette::ColorId::text, egt::Palette::black);
    label1.color(egt::Palette::ColorId::bg, egt::Palette::transparent);

    popup.add(label1);
    popup.add(label2);

    egt::experimental::CPUMonitorUsage tools;
    egt::PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &label1]()
    {
        tools.update();

        std::ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage()) << "%";
        label1.text(ss.str());
    });
    cputimer.start();

    win.add(popup);
    popup.show();

    return app.run();
}
