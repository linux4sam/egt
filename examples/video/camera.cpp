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
class MyCameraWindow : public T
{
public:
    MyCameraWindow(const Size& size)
        : T(size)
    {
        m_fscale = (double)main_screen()->size().w / (double)T::w();
        if (m_fscale <= (float)0.0)
            m_fscale = 1.0;

        T::start();
        T::show();
        T::move(Point(240, 120));
    }

    virtual void handle(Event& event) override
    {
        Window::handle(event);

        switch (event)
        {
        case eventid::pointer_dblclick:
            DBG("CameraWindow: T::scale() =  " << std::to_string(T::scale()));
            if (T::scale() < 1.0)
            {
                T::move(Point(0, 0));
                T::set_scale(m_fscale);
            }
            else
            {
                T::move(Point(240, 120));
                T::set_scale(0.60);
            }
            break;
        case eventid::pointer_drag_start:
            m_start_point = T::box().point();
            break;
        case eventid::pointer_drag:
        {
            DBG("CameraWindow: pointer_drag ");
            auto diff = event.pointer().drag_start - event.pointer().point;
            T::move(m_start_point - Point(diff.x, diff.y));
            break;
        }
        default:
            break;
        }
    }

private:
    Point m_start_point;
    double m_fscale;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "video");
    TopWindow win;
    win.set_color(Palette::ColorId::bg, Palette::black);

    shared_ptr<CameraWindow> window;
    window = make_shared<CameraWindow>(Size(320, 240));
    window->set_name("Camera");
    window->start();
    win.add(window);
    window->show();
    win.show();

    Label label("Error: ", Rect(Point(0, 0), Size(win.w() * 0.80, win.h() * 0.10)));
    label.set_color(Palette::ColorId::label_text, Palette::white);
    label.set_color(Palette::ColorId::bg, Palette::transparent);
    label.set_align(alignmask::top | alignmask::center);

    Point m_start_point;
    window->on_event([window, &win, &label, &m_start_point](Event & event)
    {
        switch (event.id())
        {
        case eventid::event2:
        {
            label.set_text("Error: " + window->get_error_message());
            win.add(label);
            break;
        }
        case eventid::pointer_drag_start:
        {
            m_start_point = window->box().point();
            break;
        }
        case eventid::pointer_drag:
        {
            DBG("CameraWindow: pointer_drag ");
            auto diff = event.pointer().drag_start - event.pointer().point;
            window->move(m_start_point - Point(diff.x, diff.y));
            break;
        }
        default:
            break;
        }
    });

    shared_ptr<Window> ctrlwindow;
    ctrlwindow = make_shared< Window>(Size(win.w() * 0.20, win.h() * 0.10));
    ctrlwindow->set_align(alignmask::top | alignmask::right);
    win.add(ctrlwindow);
    ctrlwindow->set_color(Palette::ColorId::bg, Palette::transparent); // Color(0x80808055));

    HorizontalBoxSizer hpos;
    hpos.set_align(alignmask::center);
    hpos.set_name("grid");
    ctrlwindow->add(hpos);

    ImageButton fullscreen(Image(":fullscreen_png"));
    fullscreen.set_boxtype(Theme::boxtype::none);
    hpos.add(fullscreen);

    fullscreen.on_event([&fullscreen, window](Event&)
    {
        static bool scaled = true;
        if (scaled)
        {
            window->move(Point(0, 0));
            window->set_scale(2.5);
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

    ctrlwindow->show();

    return app.run();
}
