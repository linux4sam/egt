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

// warning: not utf-8 safe
static std::string line_break(const std::string& in, size_t width = 50)
{
    std::string out;
    std::string tmp;
    char last = '\0';
    size_t i = 0;

    for (auto& cur : in)
    {
        if (++i == width)
        {
            tmp = egt::detail::ltrim(tmp);
            out += "\n" + tmp;
            i = tmp.length();
            tmp.clear();
        }
        else if (std::isspace(cur) && !std::isspace(last))
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
    egt::Application app(argc, argv, "camera");
    egt::add_search_path(EXAMPLEDATA);

    egt::Size size(320, 240);
    auto format = egt::PixelFormat::yuyv;
    std::string dev = "/dev/video0";
    if (argc == 5)
    {
        dev = std::string(argv[1]);
        size.width(atoi(argv[2]));
        size.height(atoi(argv[3]));
        if (atoi(argv[4]) <= 10)
            format = static_cast<egt::PixelFormat>(atoi(argv[4]));
    }

    egt::TopWindow win;
    win.color(egt::Palette::ColorId::bg, egt::Palette::black);

    egt::CameraWindow player(size, dev, format, egt::WindowHint::overlay);
    player.move_to_center(win.center());
    win.add(player);

    egt::Label errlabel;
    errlabel.color(egt::Palette::ColorId::label_text, egt::Palette::white);
    errlabel.align(egt::AlignFlag::expand);
    errlabel.text_align(egt::AlignFlag::center | egt::AlignFlag::top);
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

    egt::Point m_start_point;
    player.on_event([&player, &m_start_point](egt::Event & event)
    {
        switch (event.id())
        {
        case egt::EventId::pointer_drag_start:
        {
            m_start_point = player.box().point();
            break;
        }
        case egt::EventId::pointer_drag:
        {
            auto diff = event.pointer().drag_start - event.pointer().point;
            player.move(m_start_point - egt::Point(diff.x(), diff.y()));
            break;
        }
        default:
            break;
        }

        event.quit();
    });

    egt::Window ctrlwindow(egt::Size(win.width(), 72));
    ctrlwindow.align(egt::AlignFlag::bottom | egt::AlignFlag::center);
    ctrlwindow.color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    win.add(ctrlwindow);

    egt::HorizontalBoxSizer hpos;
    hpos.align(egt::AlignFlag::center);
    ctrlwindow.add(hpos);

    egt::ImageButton fullscreen(egt::Image("res:fullscreen_png"));
    fullscreen.fill_flags().clear();
    hpos.add(fullscreen);

    const auto wscale = static_cast<float>(egt::Application::instance().screen()->size().width()) / size.width();
    const auto hscale = static_cast<float>(egt::Application::instance().screen()->size().height()) / size.height();

    fullscreen.on_event([&fullscreen, &player, wscale, hscale](egt::Event&)
    {
        static bool scaled = true;
        if (scaled)
        {
            player.move(egt::Point(0, 0));
            player.scale(wscale, hscale);
            fullscreen.image(egt::Image("res:fullscreen_exit_png"));
            scaled = false;
        }
        else
        {
            player.move(egt::Point(240, 120));
            player.scale(1.0, 1.0);
            fullscreen.image(egt::Image("res:fullscreen_png"));
            scaled = true;
        }
    }, {egt::EventId::pointer_click});

    egt::Label cpulabel("CPU: 0%", egt::Size(100, 40));
    cpulabel.color(egt::Palette::ColorId::label_text, egt::Palette::white);
    hpos.add(cpulabel);

    egt::experimental::CPUMonitorUsage tools;
    egt::PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&cpulabel, &tools]()
    {
        tools.update();
        std::ostringstream ss;
        ss << "CPU: " << (int)tools.usage() << "%";
        cpulabel.text(ss.str());
    });
    cputimer.start();

    ctrlwindow.show();
    player.show();
    win.show();

    return app.run();
}
