/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cmath>
#include <egt/detail/mousegesture.h>
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
        : TopWindow(Size(), widgetmask::NO_BACKGROUND),
          m_img(Image("background.png"))
    {
        add(&m_img);
        if (m_img.w() != w())
        {
            double scale = (double)w() / (double)m_img.w();
            m_img.scale_image(scale);
        }

        auto logo = new ImageLabel(Image("@microchip_logo_white.png"));
        add(logo)->set_align(alignmask::LEFT | alignmask::TOP, 10);
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
        flag_set(widgetmask::TRANSPARENT_BACKGROUND);
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color(0x526d7480));

        add(&m_grip);
        m_grip.resize(Size(50, 50));
        m_grip.set_align(alignmask::RIGHT | alignmask::BOTTOM);

        add(&m_arrows);
        m_arrows.resize(box().size() / 2);
        m_arrows.set_align(alignmask::CENTER);
    }

    virtual int handle(eventid event) override
    {
        auto mouse = m_mouse.handle(event);
        switch (mouse)
        {
        case Swipe::mouse_event::done:
        case Swipe::mouse_event::none:
            break;
        case Swipe::mouse_event::start:
            m_mouse.start(box().point());
            return 1;
        case Swipe::mouse_event::drag:
        {
            auto diff = m_mouse.mouse_start() - event_mouse();
            move(m_mouse.start_value() - diff);
            return 1;
        }
        case Swipe::mouse_event::click:
            break;
        }

        auto ret = Window::handle(event);
        return ret;
    }

protected:
    ImageLabel m_grip;
    ImageLabel m_arrows;

    using Swipe = detail::MouseGesture<Point>;

    Swipe m_mouse;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "drag");

    MainWindow win;

    FloatingBox box1(Rect(win.w() / 5, win.h() / 3, win.w() / 5, win.h() / 3));
    win.add(&box1);

    FloatingBox box2(Rect(win.w() / 5 * 3, win.h() / 3, win.w() / 5, win.h() / 3));
    win.add(&box2);

    win.show();
    box1.show();
    box2.show();

    Label label1("CPU: -",
                 Rect(Point(10, win.size().h - 40),
                      Size(100, 40)),
                 alignmask::LEFT | alignmask::CENTER);
    label1.palette()
    .set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);

    win.add(&label1);

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

    return app.run();
}
