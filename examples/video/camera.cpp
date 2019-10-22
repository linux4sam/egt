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

    TopWindow win;
    win.set_color(Palette::ColorId::bg, Palette::black);

    CameraWindow player(Size(320, 240));
    player.move_to_center(win.center());
    player.start();
    win.add(player);

    Label errlabel;
    errlabel.set_color(Palette::ColorId::label_text, Palette::white);
    errlabel.set_align(alignmask::expand);
    errlabel.set_text_align(alignmask::center | alignmask::top);
    win.add(errlabel);

    Point m_start_point;
    player.on_event([&player, &win, &errlabel, &m_start_point](Event & event)
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

    fullscreen.on_event([&fullscreen, &player](Event&)
    {
        static bool scaled = true;
        if (scaled)
        {
            player.move(Point(0, 0));
            player.set_scale(2.5, 2.5);
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
