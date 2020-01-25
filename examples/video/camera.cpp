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
    auto format = PixelFormat::yuyv;
    std::string dev = "/dev/video0";
    if (argc == 5)
    {
        dev = std::string(argv[1]);
        size.width(atoi(argv[2]));
        size.height(atoi(argv[3]));
        if (atoi(argv[4]) <= 10)
            format = static_cast<PixelFormat>(atoi(argv[4]));
    }

    TopWindow win;
    win.color(Palette::ColorId::bg, Palette::black);

    CameraWindow player(size, dev, format, WindowHint::overlay);
    player.move_to_center(win.center());
    win.add(player);

    Label errlabel;
    errlabel.color(Palette::ColorId::label_text, Palette::white);
    errlabel.align(AlignFlag::expand);
    errlabel.text_align(AlignFlag::center | AlignFlag::top);
    win.add(errlabel);

    // wait to start playing the video until the window is shown
    win.on_show([&player]()
    {
        player.start();
    });

    player.on_error([&player, &errlabel]()
    {
        errlabel.text(line_break(player.error_message()));
    });

    Point m_start_point;
    player.on_event([&player, &m_start_point](Event & event)
    {
        switch (event.id())
        {
        case EventId::pointer_drag_start:
        {
            m_start_point = player.box().point();
            break;
        }
        case EventId::pointer_drag:
        {
            auto diff = event.pointer().drag_start - event.pointer().point;
            player.move(m_start_point - Point(diff.x(), diff.y()));
            break;
        }
        default:
            break;
        }

        event.quit();
    });

    Window ctrlwindow(Size(win.width(), 72));
    ctrlwindow.align(AlignFlag::bottom | AlignFlag::center);
    ctrlwindow.color(Palette::ColorId::bg, Palette::transparent);
    win.add(ctrlwindow);

    HorizontalBoxSizer hpos;
    hpos.align(AlignFlag::center);
    ctrlwindow.add(hpos);

    ImageButton fullscreen(Image("file:fullscreen_png"));
    fullscreen.fill_flags().clear();
    hpos.add(fullscreen);

    const auto wscale = static_cast<float>(Application::instance().screen()->size().width()) / size.width();
    const auto hscale = static_cast<float>(Application::instance().screen()->size().height()) / size.height();

    fullscreen.on_event([&fullscreen, &player, wscale, hscale](Event&)
    {
        static bool scaled = true;
        if (scaled)
        {
            player.move(Point(0, 0));
            player.scale(wscale, hscale);
            fullscreen.image(Image("file:fullscreen_exit_png"));
            scaled = false;
        }
        else
        {
            player.move(Point(240, 120));
            player.scale(1.0, 1.0);
            fullscreen.image(Image("file:fullscreen_png"));
            scaled = true;
        }
    }, {EventId::pointer_click});

    Label cpulabel("CPU: 0%", Size(100, 40));
    cpulabel.color(Palette::ColorId::label_text, Palette::white);
    hpos.add(cpulabel);

    egt::experimental::CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&cpulabel, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << (int)tools.usage(0) << "%";
        cpulabel.text(ss.str());
    });
    cputimer.start();

    ctrlwindow.show();
    player.show();
    win.show();

    return app.run();
}
