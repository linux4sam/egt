/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <chrono>
#include <egt/detail/string.h>
#include <egt/ui>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace egt;

// warning: not utf-8 safe
static std::string line_break(const std::string& in, size_t width = 50)
{
    string out;
    string tmp;
    char last = '\0';
    size_t i = 0;

    for (auto& cur : in)
    {
        if (++i == width)
        {
            tmp = detail::ltrim(tmp);
            out += "\n" + tmp;
            i = tmp.length();
            tmp.clear();
        }
        else if (isspace(cur) && !isspace(last))
        {
            out += tmp;
            tmp.clear();
        }
        tmp += cur;
        last = cur;
    }
    return out + tmp;
}

int main(int argc, const char** argv)
{
    Application app(argc, argv, "camera");

    Size size(320, 240);
    auto format = pixel_format::yuyv;
    std::string dev = "/dev/video0";
    if (argc == 5)
    {
        dev = std::string(argv[1]);
        size.set_width(atoi(argv[2]));
        size.set_height(atoi(argv[3]));
        if (atoi(argv[4]) <= 10)
            format = static_cast<pixel_format>(atoi(argv[4]));
    }

    TopWindow win;
    win.set_color(Palette::ColorId::bg, Palette::black);

    Label errlabel;
    errlabel.set_color(Palette::ColorId::label_text, Palette::white);
    errlabel.set_align(alignmask::expand);
    errlabel.set_text_align(alignmask::center | alignmask::top);
    win.add(errlabel);

    CameraWindow player(size, dev, format, windowhint::overlay);
    player.move_to_center(win.center());
    win.add(player);

    // wait to start playing the video until the window is shown
    win.on_event([&player](Event&)
    {
        player.start();
    }, {eventid::show});

    Point m_start_point;
    player.on_event([&player, &errlabel, &m_start_point](Event & event)
    {
        switch (event.id())
        {
        case eventid::event2:
        {
            errlabel.set_text("Error:\n" + line_break(player.error_message()));
            break;
        }
        case eventid::pointer_drag_start:
        {
            m_start_point = player.box().point();
            break;
        }
        case eventid::pointer_drag:
        {
            auto diff = event.pointer().drag_start - event.pointer().point;
            player.move(m_start_point - Point(diff.x(), diff.y()));
            break;
        }
        default:
            break;
        }
    });

    Window ctrlwindow(Size(win.width(), 72));
    ctrlwindow.set_align(alignmask::bottom | alignmask::center);
    ctrlwindow.set_color(Palette::ColorId::bg, Palette::transparent);
    win.add(ctrlwindow);

    HorizontalBoxSizer hpos;
    hpos.set_align(alignmask::center);
    ctrlwindow.add(hpos);

    ImageButton fullscreen(Image(":fullscreen_png"));
    fullscreen.set_boxtype(Theme::boxtype::none);
    hpos.add(fullscreen);

    const auto wscale = static_cast<float>(Application::instance().screen()->size().width()) / size.width();
    const auto hscale = static_cast<float>(Application::instance().screen()->size().height()) / size.height();

    fullscreen.on_event([&fullscreen, &player, wscale, hscale](Event&)
    {
        static bool scaled = true;
        if (scaled)
        {
            player.move(Point(0, 0));
            player.set_scale(wscale, hscale);
            fullscreen.set_image(Image(":fullscreen_exit_png"));
            scaled = false;
        }
        else
        {
            player.move(Point(240, 120));
            player.set_scale(1.0, 1.0);
            fullscreen.set_image(Image(":fullscreen_png"));
            scaled = true;
        }
    }, {eventid::pointer_click});

    Label cpulabel("CPU: 0%", Size(100, 40));
    cpulabel.set_color(Palette::ColorId::label_text, Palette::white);
    hpos.add(cpulabel);

    experimental::CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&cpulabel, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << (int)tools.usage(0) << "%";
        cpulabel.set_text(ss.str());
    });
    cputimer.start();

    ctrlwindow.show();
    player.show();
    win.show();

    return app.run();
}
