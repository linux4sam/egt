/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace egt;

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        cerr << argv[0] << " FILENAME" << endl;
        return 1;
    }

    int w, h;
    if (argc == 4)
    {
        w = atoi(argv[2]);
        h = atoi(argv[3]);
    }
    else
    {
        w = 320;
        h = 192;
    }

    Application app(argc, argv, "video");
    TopWindow win;
    win.set_color(Palette::ColorId::bg, Palette::black);

    auto window = make_shared<VideoWindow>(Rect(0, 0, w, h), pixel_format::yuv420, windowhint::overlay);
    window->set_media(argv[1]);
    window->set_name("video");
    window->move_to_center(win.center());
    window->play();
    window->set_volume(5);
    win.add(window);

    Label label("Error: ", Rect(Point(0, 0), Size(win.w() * 0.80, win.h() * 0.10)));
    label.set_color(Palette::ColorId::label_text, Palette::white);
    label.set_color(Palette::ColorId::bg, Palette::transparent);
    label.set_align(alignmask::top | alignmask::center);

    window->on_event([window, &win, &label](Event&)
    {
        label.set_text("Error: " + window->get_error_message());
        win.add(label);
    }, {eventid::event2});

    auto ctrlwindow = make_shared< Window>(Size(win.w(), 72));
    ctrlwindow->set_align(alignmask::bottom | alignmask::center);
    win.add(ctrlwindow);
    ctrlwindow->set_color(Palette::ColorId::bg, Palette::transparent);

    HorizontalBoxSizer hpos;
    hpos.resize(ctrlwindow->size());
    hpos.set_name("grid");
    ctrlwindow->add(hpos);

    ImageButton playbtn(Image(":pause_png"));
    playbtn.set_boxtype(Theme::boxtype::none);
    hpos.add(playbtn);

    playbtn.on_event([&playbtn, window](Event&)
    {
        if (window->playing())
        {
            if (window->pause())
                playbtn.set_image(Image(":play_png"));
        }
        else
        {
            if (window->play())
                playbtn.set_image(Image(":pause_png"));
        }
    }, {eventid::pointer_click});

    Slider position(0, 100, 0, orientation::horizontal);
    position.set_width(ctrlwindow->w() * 0.20);
    position.set_align(alignmask::expand_vertical);
    hpos.add(position);
    position.set_color(Palette::ColorId::text_highlight, Palette::blue);
    position.slider_flags().set({Slider::flag::round_handle});

    position.on_event([&position, window](Event&)
    {
        if (window->playing())
        {
            window->seek((window->duration() * position.value()) / position.max());
        }
        return 0;
    }, {eventid::property_changed});

    ImageButton volumei(Image(":volumeup_png"));
    volumei.set_boxtype(Theme::boxtype::none);
    hpos.add(volumei);

    Slider volume(Rect(Size(ctrlwindow->w() * 0.10, ctrlwindow->h())), 0, 10, 0, orientation::horizontal);
    hpos.add(volume);
    volume.slider_flags().set({Slider::flag::round_handle});
    volume.set_value(5);
    window->set_volume(5.0);
    volume.on_event([&volume, window](Event&)
    {
        auto val = static_cast<double>(volume.value());
        window->set_volume(val);
    });
    volume.set_value(5);

    ImageButton fullscreen(Image(":fullscreen_png"));
    fullscreen.set_boxtype(Theme::boxtype::none);
    hpos.add(fullscreen);

    double m_fscale = (double)main_screen()->size().w / w;

    fullscreen.on_event([&fullscreen, window, m_fscale, &win](Event&)
    {
        static bool scaled = true;
        if (scaled)
        {
            window->move(Point(0, 0));
            window->set_scale(m_fscale);
            fullscreen.set_image(Image(":fullscreen_exit_png"));
            scaled = false;
        }
        else
        {
            window->move_to_center(win.center());
            window->set_scale(1.0);
            fullscreen.set_image(Image(":fullscreen_png"));
            scaled = true;
        }
    }, {eventid::pointer_click});

    ImageButton loopback(Image(":repeat_one_png"));
    loopback.set_boxtype(Theme::boxtype::none);
    hpos.add(loopback);

    loopback.on_event([&loopback, window](Event&)
    {
        if (window->get_loopback())
        {
            loopback.set_image(Image(":repeat_one_png"));
            window->set_loopback(false);
        }
        else
        {
            loopback.set_image(Image(":repeat_png"));
            window->set_loopback(true);
        }
        return 0;
    }, {eventid::pointer_click});

    Label label1("CPU: 0%", Rect(Point(0, 0), Size(100, 40)));
    label1.set_color(Palette::ColorId::label_text, Palette::white);
    label1.set_color(Palette::ColorId::bg, Palette::transparent);
    hpos.add(label1);

    experimental::CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&label1, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << (int)tools.usage(0) << "%";
        label1.set_text(ss.str());
    });
    cputimer.start();

    window->on_event([window, &win, &label, &position](Event & event)
    {
        static Point m_start_point;
        switch (event.id())
        {
        case eventid::pointer_drag_start:
        {
            m_start_point = window->box().point();
            break;
        }
        case eventid::pointer_drag:
        {
            auto diff = event.pointer().drag_start - event.pointer().point;
            window->move(m_start_point - Point(diff.x, diff.y));
            break;
        }
        case eventid::property_changed:
        {
            if (window->playing())
            {
                double v = (double)window->position() / (double)window->duration() * 100.;
                position.set_value(v);
            }
            else
            {
                position.set_value(0);
            }
            break;
        }
        case eventid::event2:
        {
            label.set_text("Error: " + window->get_error_message());
            win.add(label);
            break;
        }
        default:
            break;
        }
    });

    ctrlwindow->show();
    window->show();
    win.show();

    return app.run();
}
