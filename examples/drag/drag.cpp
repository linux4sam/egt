/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
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

class MainWindow : public Window
{
public:
    MainWindow()
        : Window(Size(), widgetmask::NO_BACKGROUND),
          m_img("background.png")
    {
        add(&m_img);
        if (m_img.w() != w())
        {
            double scale = (double)w() / (double)m_img.w();
            m_img.scale(scale, scale);
        }

        auto logo = new Image("@microchip_logo_white.png");
        add(logo)->set_align(alignmask::LEFT | alignmask::TOP, 10);
    }

    Image m_img;
};

class FloatingBox : public Frame
{
public:
    FloatingBox(const Rect& rect)
        : Frame(rect),
          m_grip("grip.png"),
          m_arrows("arrows.png")
    {
        flag_set(widgetmask::TRANSPARENT_BACKGROUND);
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color(0x526d7480));

        add(&m_grip);
        m_grip.resize(Size(50,50));
        m_grip.set_align(alignmask::RIGHT | alignmask::BOTTOM);

        add(&m_arrows);
        m_arrows.resize(box().size() / 2);
        m_arrows.set_align(alignmask::CENTER);
    }

    virtual int handle(eventid event) override
    {
        auto ret = Frame::handle(event);
        if (ret)
            return ret;

        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            m_starting_pos = box().point();
            m_starting = event_mouse();
            m_moving = true;
            return 1;
            break;
        }
        case eventid::MOUSE_UP:
            m_moving = false;
            return 1;
        case eventid::MOUSE_MOVE:
            if (m_moving)
            {
                auto diff = m_starting - event_mouse();
                move(m_starting_pos - diff);
                return 1;
            }
            break;
        default:
            break;
        }

        return 0;
    }

protected:
    Image m_grip;
    Image m_arrows;
    bool m_moving{false};
    Point m_starting;
    Point m_starting_pos;
};

int main()
{
    Application app;

    set_image_path("../share/egt/examples/drag/");

    MainWindow win;

    FloatingBox box1(Rect(win.w()/5, win.h()/3, win.w()/5, win.h()/3));
    win.add(&box1);

    FloatingBox box2(Rect(win.w()/5 * 3, win.h()/3, win.w()/5, win.h()/3));
    win.add(&box2);

    win.show();

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
