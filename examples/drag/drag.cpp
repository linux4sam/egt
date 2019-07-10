/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cmath>
#include <egt/ui>
#include <egt/detail/math.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace egt;
using namespace egt::detail;

class MainWindow : public TopWindow
{
public:
    MainWindow()
        : TopWindow(Size()),
          m_img(*this, Image("background.png"))
    {
        set_boxtype(Theme::boxtype::none);
        m_img.set_align(alignmask::expand);
        m_img.set_image_align(alignmask::expand);

        auto logo = make_shared<ImageLabel>(Image("@128px/microchip_logo_white.png"));
        logo->set_align(alignmask::left | alignmask::top);
        logo->set_margin(10);
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
        set_color(Palette::ColorId::bg, Color(0x526d7480));
        set_color(Palette::ColorId::bg, Color(0xff6d7480), Palette::GroupId::active);

        add(m_grip);
        m_grip.resize(Size(50, 50));
        m_grip.set_align(alignmask::right | alignmask::bottom);

        add(m_arrows);
        m_arrows.resize(box().size() / 2);
        m_arrows.image().resize(box().size() / 2);
        m_arrows.set_align(alignmask::center);
    }

    virtual void handle(Event& event) override
    {
        Window::handle(event);

        switch (event.id())
        {
        case eventid::pointer_drag_start:
            m_start_point = box().point();
            break;
        case eventid::pointer_drag:
        {
            auto diff = event.pointer().drag_start - event.pointer().point;
            move(m_start_point - Point(diff.x(), diff.y()));
            break;
        }
        default:
            break;
        }
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

    FloatingBox box1(Rect(Ratio<int>(win.width(), 20),
                          Ratio<int>(win.height(), 20),
                          Ratio<int>(win.width(), 20),
                          Ratio<int>(win.width(), 20)));
    win.add(box1);

    FloatingBox box2(Rect(Ratio<int>(win.width(), 20) * 3,
                          Ratio<int>(win.height(), 20),
                          Ratio<int>(win.width(), 20),
                          Ratio<int>(win.width(), 20)));
    win.add(box2);

    box1.show();
    box2.show();

    Label label1("CPU: ----", Rect(), alignmask::left | alignmask::center);
    label1.set_align(alignmask::left | alignmask::bottom);
    label1.set_color(Palette::ColorId::text, Palette::white);
    label1.set_color(Palette::ColorId::bg, Palette::transparent);

    win.add(label1);

    experimental::CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &label1]()
    {
        tools.update();

        ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage(0)) << "%";
        label1.set_text(ss.str());
    });
    cputimer.start();

    win.show();

    return app.run();
}
