/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cmath>
#include <egt/ui>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

class MainWindow : public TopWindow
{
public:
    MainWindow()
        : TopWindow(Size()),
          m_img(Image("background.png"))
    {
        set_boxtype(Theme::boxtype::none);
        add(m_img);
        if (m_img.w() != w())
        {
            double scale = (double)w() / (double)m_img.w();
            m_img.scale_image(scale);
        }

        auto logo = make_shared<ImageLabel>(Image("@microchip_logo_white.png"));
        logo->set_align(alignmask::left | alignmask::top, 10);
        add(logo);
    }

    ImageLabel m_img;
};

class FloatingBox : public Window
{
public:
    explicit FloatingBox(const Rect& rect)
        : Window(rect),
          m_grip(Image("grip.png")),
          m_arrows(Image("arrows.png"))
    {
        flags().set(Widget::flag::grab_mouse);
        set_boxtype(Theme::boxtype::fill);
        palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Color(0x526d7480));

        add(m_grip);
        m_grip.resize(Size(50, 50));
        m_grip.set_align(alignmask::right | alignmask::bottom);

        add(m_arrows);
        m_arrows.resize(box().size() / 2);
        m_arrows.image().resize(box().size() / 2);
        m_arrows.set_align(alignmask::center);
    }

    virtual int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::pointer_drag_start:
            m_start_point = box().point();
            break;
        case eventid::pointer_drag:
        {
            auto diff = event::pointer().drag_start - event::pointer().point;
            move(m_start_point - Point(diff.x, diff.y));
            break;
        }
        default:
            break;
        }

        return Window::handle(event);
    }

protected:
    ImageLabel m_grip;
    ImageLabel m_arrows;
    Point m_start_point;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "drag");

    MainWindow win;

    FloatingBox box1(Rect(win.w() / 5, win.h() / 3, win.w() / 5, win.h() / 3));
    win.add(box1);

    FloatingBox box2(Rect(win.w() / 5 * 3, win.h() / 3, win.w() / 5, win.h() / 3));
    win.add(box2);

    box1.show();
    box2.show();

    Label label1("CPU: ----", Rect(), alignmask::left | alignmask::center);
    label1.set_align(alignmask::left | alignmask::bottom);
    label1.palette()
    .set(Palette::ColorId::text, Palette::GroupId::normal, Palette::white)
    .set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::transparent);

    win.add(label1);

    CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &label1]()
    {
        tools.update();

        ostringstream ss;
        ss << "CPU: " << (int)tools.usage(0) << "%";
        label1.set_text(ss.str());
    });
    cputimer.start();

    win.show();

    return app.run();
}
