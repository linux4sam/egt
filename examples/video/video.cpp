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

template <class T>
class MyVideoWindow : public T
{
public:
    MyVideoWindow(const Size& size, const string& filename)
        : T(size, pixel_format::yuv420, windowhint::overlay),
          m_moving(true),
          m_scaled(false)
    {
        m_fscale = (double)main_screen()->size().w / (double)T::w();
        if (m_fscale <= 0)
            m_fscale = 1.0;

        T::set_media(filename);
        T::play();
        T::set_volume(5);
    }

    MyVideoWindow(const Rect& rect, const string& filename)
        : T(rect, pixel_format::yuv420, windowhint::overlay),
          m_moving(true),
          m_scaled(false)
    {
        m_fscale = (double)main_screen()->size().w / (double)T::w();
        if (m_fscale <= 0)
            m_fscale = 1.0;

        T::set_media(filename);
        T::play();
        T::set_volume(5);
    }


    virtual int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::pointer_dblclick:
        {
            DBG("VideoWindow: pointer_dblclick ");
            if (!m_scaled)
            {
                T::move(Point(0, 0));
                T::set_scale(m_fscale);
                m_scaled = true;
            }
            else
            {
                T::set_scale(0.80);
                m_scaled = false;
            }
            break;
        }
        case eventid::pointer_drag_start:
            m_start_point = T::box().point();
            break;
        case eventid::pointer_drag:
        {
            DBG("VideoWindow: pointer_drag ");
            auto diff = event::pointer().drag_start - event::pointer().point;
            T::move(m_start_point - Point(diff.x, diff.y));
            break;
        }
        case eventid::property_changed:
        {
            DBG("VideoWindow: property_changed");
            break;
        }
        case eventid::event2:
        {
            DBG("VideoWindow: event2 : Error");
            break;
        }
        default:
            break;
        }
        return Window::handle(event);
    }

private:
    bool m_moving;
    bool m_scaled;
    Point m_start_point;
    Point m_position;
    double m_fscale;
};

class FpsWindow : public Window
{
public:
    FpsWindow()
        : Window(Size(100, 50),
                 Widget::flags_type(), pixel_format::argb8888),
          m_label(make_shared<Label>("FPS: 0",
                                     Rect(Size(100, 50))))
    {
        m_label->set_color(Palette::ColorId::text, Palette::white);
        add(m_label);
    }

    void set_text(const std::string& str)
    {
        m_label->set_text(str);
    }

    shared_ptr<Label> m_label;
};

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

    shared_ptr<VideoWindow> window;
    window = make_shared<MyVideoWindow<VideoWindow>>(Rect(0, 0, w, h), argv[1]);
    window->set_name("video");
    window->set_align(alignmask::center);
    win.add(window);

    Label label("Error: ", Rect(Point(0, 0), Size(win.w() * 0.80, win.h() * 0.10)));
    label.set_color(Palette::ColorId::label_text, Palette::white);
    label.set_color(Palette::ColorId::bg, Palette::transparent);
    label.set_align(alignmask::top | alignmask::center);

    window->on_event([window, &win, &label](eventid)
    {
        label.set_text("Error: " + window->get_error_message());
        win.add(label);
        return 0;
    }, {eventid::event2});

    shared_ptr<Window> ctrlwindow;
    ctrlwindow = make_shared< Window>(Size(win.w(), win.h() * 0.12));
    ctrlwindow->set_align(alignmask::bottom | alignmask::center);
    win.add(ctrlwindow);
    ctrlwindow->set_color(Palette::ColorId::bg, Palette::transparent); // Color(0x80808055));

    HorizontalPositioner hpos(Rect(0, 0, ctrlwindow->w(), ctrlwindow->h()), 3, alignmask::center);
    hpos.set_name("grid");
    ctrlwindow->add(hpos);

    ImageButton playbtn(Image(":pause_png"));
    playbtn.set_boxtype(Theme::boxtype::none);
    hpos.add(playbtn);

    playbtn.on_event([&playbtn, window](eventid)
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
        return 0;
    }, {eventid::pointer_click});

    Slider position(Rect(Size(ctrlwindow->w() * 0.40, ctrlwindow->h())), 0, 100, 0, orientation::horizontal);
    hpos.add(position);
    position.set_color(Palette::ColorId::text_highlight, Palette::blue);
    position.slider_flags().set({Slider::flag::round_handle});
    position.set_readonly(0);
    position.disable();

    PeriodicTimer postimer(std::chrono::milliseconds(200));
    postimer.on_timeout([&position, window]()
    {
        if (window->duration())
        {
            double v = (double)window->position() / (double)window->duration() * 100.;
            position.set_value(v);
        }
        else
        {
            position.set_value(0);
        }
    });
    postimer.start();

    ImageButton volumei(Image(":volumeup_png"));
    volumei.set_boxtype(Theme::boxtype::none);
    hpos.add(volumei);

    Slider volume(Rect(Size(ctrlwindow->w() * 0.10, ctrlwindow->h())), 0, 10, 0, orientation::horizontal);
    hpos.add(volume);
    volume.slider_flags().set({Slider::flag::round_handle});
    volume.set_value(5);
    window->set_volume(5.0);
    volume.on_event([&volume, window](eventid)
    {
        window->set_volume(volume.value());
        return 0;
    });
    volume.set_value(5);

    ImageButton fullscreen(Image(":fullscreen_png"));
    fullscreen.set_boxtype(Theme::boxtype::none);
    hpos.add(fullscreen);

    double m_fscale = (double)main_screen()->size().w / w;

    fullscreen.on_event([&fullscreen, window, m_fscale](eventid)
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
            window->move(Point(240, 120)); //set center
            window->set_scale(1.0);
            fullscreen.set_image(Image(":fullscreen_png"));
            scaled = true;
        }
        return 0;
    }, {eventid::pointer_click});

    ImageButton loopback(Image(":repeat_one_png"));
    loopback.set_boxtype(Theme::boxtype::none);
    hpos.add(loopback);

    loopback.on_event([&loopback, window](eventid)
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

    CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&label1, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << (int)tools.usage(0) << "%";
        label1.set_text(ss.str());
    });
    cputimer.start();

    hpos.reposition();
    ctrlwindow->show();
    window->show();
    win.show();

    return app.run();
}
