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

template<class T>
static inline T ns2ms(T n)
{
    return n / 1000000UL;
}

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        cerr << argv[0] << " FILENAME" << endl;
        return 1;
    }

    Size size(320, 192);
    if (argc == 4)
    {
        size.set_width(atoi(argv[2]));
        size.set_height(atoi(argv[3]));
    }

    Application app(argc, argv, "video");
    TopWindow win;
    win.set_color(Palette::ColorId::bg, Palette::black);

    Label errlabel;
    errlabel.set_color(Palette::ColorId::label_text, Palette::white);
    errlabel.set_align(alignmask::expand);
    errlabel.set_text_align(alignmask::center | alignmask::top);
    win.add(errlabel);

    // player after label to handle drag
    VideoWindow player(size, pixel_format::yuv420, windowhint::overlay);
    player.move_to_center(win.center());
    player.set_volume(5);
    win.add(player);

    // wait to start playing the video until the window is shown
    win.on_event([&player, argv](Event&)
    {
        player.set_media(argv[1]);
        player.play();
    }, {eventid::show});

    Window ctrlwindow(Size(win.width(), 72));
    ctrlwindow.set_align(alignmask::bottom | alignmask::center);
    ctrlwindow.set_color(Palette::ColorId::bg, Palette::transparent);
    win.add(ctrlwindow);

    HorizontalBoxSizer hpos;
    hpos.resize(ctrlwindow.size());
    ctrlwindow.add(hpos);

    auto logo = make_shared<ImageLabel>(Image("@32px/egt_logo_icon.png"));
    logo->set_margin(10);
    hpos.add(logo);

    ImageButton playbtn(Image(":pause_png"));
    playbtn.set_boxtype(Theme::boxtype::none);
    hpos.add(playbtn);

    playbtn.on_event([&playbtn, &player](Event&)
    {
        if (player.playing())
        {
            if (player.pause())
                playbtn.set_image(Image(":play_png"));
        }
        else
        {
            if (player.play())
                playbtn.set_image(Image(":pause_png"));
        }
    }, {eventid::pointer_click});

    Slider position(0, 100, 0, orientation::horizontal);
    position.set_width(ctrlwindow.width() * 0.20);
    position.set_align(alignmask::expand_vertical);
    position.slider_flags().set({Slider::flag::round_handle});
    hpos.add(position);

    position.on_event([&position, &player](Event&)
    {
        if (player.playing())
        {
            player.seek((player.duration() * position.value()) / position.max());
        }
    }, {eventid::property_changed});

    ImageButton volumei(Image(":volumeup_png"));
    volumei.set_boxtype(Theme::boxtype::none);
    hpos.add(volumei);

    Slider volume(Size(ctrlwindow.width() * 0.10, ctrlwindow.height()), 0, 10, 0, orientation::horizontal);
    hpos.add(volume);
    volume.slider_flags().set({Slider::flag::round_handle});
    volume.set_value(5);
    player.set_volume(5.0);
    volume.on_event([&volume, &player](Event&)
    {
        auto val = static_cast<double>(volume.value());
        player.set_volume(val);
    });
    volume.set_value(5);

    ImageButton fullscreenbtn(Image(":fullscreen_png"));
    fullscreenbtn.set_boxtype(Theme::boxtype::none);
    hpos.add(fullscreenbtn);

    const float vscale = (float)main_screen()->size().width() / size.width();

    fullscreenbtn.on_event([&fullscreenbtn, &player, vscale, &win](Event&)
    {
        static bool scaled = true;
        if (scaled)
        {
            player.move(Point(0, 0));
            player.set_scale(vscale, vscale);
            fullscreenbtn.set_image(Image(":fullscreen_exit_png"));
            scaled = false;
        }
        else
        {
            player.move_to_center(win.center());
            player.set_scale(1.0, 1.0);
            fullscreenbtn.set_image(Image(":fullscreen_png"));
            scaled = true;
        }
    }, {eventid::pointer_click});

    ImageButton loopbackbtn(Image(":repeat_one_png"));
    loopbackbtn.set_boxtype(Theme::boxtype::none);
    hpos.add(loopbackbtn);

    loopbackbtn.on_event([&loopbackbtn, &player](Event&)
    {
        if (player.loopback())
        {
            loopbackbtn.set_image(Image(":repeat_one_png"));
            player.set_loopback(false);
        }
        else
        {
            loopbackbtn.set_image(Image(":repeat_png"));
            player.set_loopback(true);
        }
    }, {eventid::pointer_click});

    Label cpulabel("CPU: 0%");
    cpulabel.set_color(Palette::ColorId::label_text, Palette::white);
    hpos.add(cpulabel);

    experimental::CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&cpulabel, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage(0)) << "%";
        cpulabel.set_text(ss.str());
    });
    cputimer.start();

    player.on_event([&player, &win, &errlabel, &position, vscale, size](Event & event)
    {
        static Point m_start_point;
        switch (event.id())
        {
        case eventid::pointer_drag_start:
        {
            m_start_point = player.box().point();
            break;
        }
        case eventid::pointer_drag:
        {
            if (!(detail::float_compare(player.scalex(), vscale)))
            {
                auto diff = event.pointer().drag_start - event.pointer().point;
                auto p = m_start_point - Point(diff.x(), diff.y());
                auto max_x = win.width() - size.width();
                auto max_y = win.height() - size.height();
                if (p.x() >= max_x)
                    p.set_x(max_x);
                if (p.x() < 0)
                    p.set_x(0);
                if (p.y() >= max_y)
                    p.set_y(max_y);
                if (p.y() < 0)
                    p.set_y(0);
                player.move(p);
            }
            break;
        }
        case eventid::property_changed:
        {
            if (player.playing())
            {
                position.set_value((ns2ms<double>(player.position()) /
                                    ns2ms<double>(player.duration())) * 100.);
            }
            else
                position.set_value(0);
            break;
        }
        case eventid::event2:
        {
            cout << player.error_message() << endl;
            errlabel.set_text("Error:\n" + line_break(player.error_message()));
            break;
        }
        default:
            break;
        }
    });

    ctrlwindow.show();
    player.show();
    win.show();

    return app.run();
}
