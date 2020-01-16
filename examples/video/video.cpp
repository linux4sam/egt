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
    auto format = PixelFormat::yuv420;
    if (argc == 5)
    {
        size.width(atoi(argv[2]));
        size.height(atoi(argv[3]));
        if (atoi(argv[4]) <= 10)
            format = static_cast<PixelFormat>(atoi(argv[4]));
    }

    Application app(argc, argv, "video");
    TopWindow win;
    win.color(Palette::ColorId::bg, Palette::black);

    Label errlabel;
    errlabel.color(Palette::ColorId::label_text, Palette::white);
    errlabel.align(AlignFlag::expand);
    errlabel.text_align(AlignFlag::center | AlignFlag::top);
    win.add(errlabel);

    // player after label to handle drag
    VideoWindow player(size, format, WindowHint::overlay);
    player.move_to_center(win.center());
    player.volume(5);
    win.add(player);

    Window ctrlwindow(Size(win.width(), 72));
    ctrlwindow.align(AlignFlag::bottom | AlignFlag::center);
    ctrlwindow.color(Palette::ColorId::bg, Palette::transparent);
    win.add(ctrlwindow);

    HorizontalBoxSizer hpos;
    hpos.resize(ctrlwindow.size());
    ctrlwindow.add(hpos);

    auto logo = make_shared<ImageLabel>(Image("@32px/egt_logo_icon.png"));
    logo->margin(10);
    hpos.add(logo);

    ImageButton playbtn(Image(":pause_png"));
    playbtn.boxtype().clear();
    hpos.add(playbtn);

    playbtn.on_event([&playbtn, &player](Event&)
    {
        if (player.playing())
        {
            if (player.pause())
                playbtn.image(Image(":play_png"));
        }
        else
        {
            if (player.play())
                playbtn.image(Image(":pause_png"));
        }
    }, {EventId::pointer_click});

    Slider position(0, 100, 0, Orientation::horizontal);
    position.width(ctrlwindow.width() * 0.20);
    position.align(AlignFlag::expand_vertical);
    position.slider_flags().set({Slider::SliderFlag::round_handle});
    hpos.add(position);

    position.on_value_changed([&position, &player]()
    {
        auto state = player.playing();
        if (state)
            player.pause();

        player.seek((player.duration() * position.value()) / position.max());

        if (state)
            player.play();

    });

    ImageButton volumei(Image(":volumeup_png"));
    volumei.boxtype().clear();
    hpos.add(volumei);

    Slider volume(Size(ctrlwindow.width() * 0.10, ctrlwindow.height()), 0, 10, 0, Orientation::horizontal);
    hpos.add(volume);
    volume.slider_flags().set({Slider::SliderFlag::round_handle});
    volume.value(5);
    player.volume(5.0);
    volume.on_event([&volume, &player](Event&)
    {
        auto val = static_cast<double>(volume.value());
        player.volume(val);
    });
    volume.value(5);

    ImageButton fullscreenbtn(Image(":fullscreen_png"));
    fullscreenbtn.boxtype().clear();
    hpos.add(fullscreenbtn);

    const auto vscale = static_cast<float>(Application::instance().screen()->size().width()) / size.width();

    fullscreenbtn.on_event([&fullscreenbtn, &player, vscale, &win](Event&)
    {
        static bool scaled = true;
        if (scaled)
        {
            player.move(Point(0, 0));
            player.scale(vscale, vscale);
            fullscreenbtn.image(Image(":fullscreen_exit_png"));
            scaled = false;
        }
        else
        {
            player.move_to_center(win.center());
            player.scale(1.0, 1.0);
            fullscreenbtn.image(Image(":fullscreen_png"));
            scaled = true;
        }
    }, {EventId::pointer_click});

    ImageButton loopbackbtn(Image(":repeat_one_png"));
    loopbackbtn.boxtype().clear();
    hpos.add(loopbackbtn);

    loopbackbtn.on_event([&loopbackbtn, &player](Event&)
    {
        if (player.loopback())
        {
            loopbackbtn.image(Image(":repeat_one_png"));
            player.loopback(false);
        }
        else
        {
            loopbackbtn.image(Image(":repeat_png"));
            player.loopback(true);
        }
    }, {EventId::pointer_click});

    Label cpulabel("CPU: 0%");
    cpulabel.color(Palette::ColorId::label_text, Palette::white);
    hpos.add(cpulabel);

    egt::experimental::CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&cpulabel, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage(0)) << "%";
        cpulabel.text(ss.str());
    });
    cputimer.start();

    // wait to start playing the video until the window is shown
    win.on_show([&player, argv, &position, &ctrlwindow, &volume, &volumei, &hpos]()
    {
        player.media(argv[1]);

        if (!player.has_audio())
        {
            position.width(ctrlwindow.width() * 0.45);
            hpos.remove(&volume);
            hpos.remove(&volumei);
        }

        player.play();
    });

    player.on_position_changed([&player, &position]()
    {
        if (player.playing())
        {
            position.value((ns2ms<double>(player.position()) /
                            ns2ms<double>(player.duration())) * 100.);
        }
    });

    player.on_error([&player, &errlabel]()
    {
        errlabel.text(line_break("Error: " + player.error_message()));
    });

    player.on_event([&player, &win, vscale, size](Event & event)
    {
        static Point m_start_point;
        switch (event.id())
        {
        case EventId::pointer_drag_start:
        {
            m_start_point = player.box().point();
            break;
        }
        case EventId::pointer_drag:
        {
            if (!(detail::float_compare(player.hscale(), vscale)))
            {
                auto diff = event.pointer().drag_start - event.pointer().point;
                auto p = m_start_point - Point(diff.x(), diff.y());
                auto max_x = win.width() - size.width();
                auto max_y = win.height() - size.height();
                if (p.x() >= max_x)
                    p.x(max_x);
                if (p.x() < 0)
                    p.x(0);
                if (p.y() >= max_y)
                    p.y(max_y);
                if (p.y() < 0)
                    p.y(0);
                player.move(p);
            }
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
