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

class FloatingBox : public Window
{
public:
    explicit FloatingBox(const Rect& rect)
        : Window(rect),
          m_grip(Image("grip.png")),
          m_arrows(Image("arrows.png"))
    {
        flags().set(Widget::Flag::grab_mouse);
        color(Palette::ColorId::bg, Color(0x526d7480));
        color(Palette::ColorId::bg, Color(0xff6d7480), Palette::GroupId::active);

        add(m_grip);
        m_grip.resize(Size(50, 50));
        m_grip.align(AlignFlag::right | AlignFlag::bottom);

        add(m_arrows);
        m_arrows.resize(box().size() / 2);
        m_arrows.image().resize(box().size() / 2);
        m_arrows.align(AlignFlag::center);
    }

    virtual void handle(Event& event) override
    {
        Window::handle(event);

        switch (event.id())
        {
        case EventId::pointer_drag_start:
            m_start_point = box().point();
            break;
        case EventId::pointer_drag:
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
    PropertyAnimator m_animationx;
    PropertyAnimator m_animationy;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "drag");

    TopWindow window;
    window.background(Image("background.png"));

    FloatingBox box1(Rect(Ratio<int>(window.width(), 20),
                          Ratio<int>(window.height(), 20),
                          Ratio<int>(window.width(), 20),
                          Ratio<int>(window.width(), 20)));
    window.add(box1);

    FloatingBox box2(Rect(Ratio<int>(window.width(), 20) * 3,
                          Ratio<int>(window.height(), 20),
                          Ratio<int>(window.width(), 20),
                          Ratio<int>(window.width(), 20)));
    window.add(box2);

    box1.show();
    box2.show();

    Label label1("CPU: ----", Rect(), AlignFlag::left | AlignFlag::center);
    label1.align(AlignFlag::left | AlignFlag::bottom);
    label1.color(Palette::ColorId::label_text, Palette::white);
    label1.color(Palette::ColorId::bg, Palette::transparent);
    window.add(label1);

    ImageLabel logo(Image("@128px/egt_logo_white.png"));
    logo.margin(10);
    window.add(center(top(logo)));

    egt::experimental::CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &label1]()
    {
        tools.update();

        ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage(0)) << "%";
        label1.text(ss.str());
    });
    cputimer.start();

    window.show();

    return app.run();
}
