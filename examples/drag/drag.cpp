/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <chrono>
#include <egt/ui>
#include <egt/detail/math.h>
#include <sstream>

class FloatingBox : public egt::Window
{
public:
    explicit FloatingBox(const egt::Rect& rect)
        : egt::Window(rect),
          m_grip(egt::Image("file:grip.png")),
          m_arrows(egt::Image("file:arrows.png"))
    {
        flags().set(egt::Widget::Flag::grab_mouse);
        color(egt::Palette::ColorId::bg, egt::Color(0x526d7480));
        color(egt::Palette::ColorId::bg, egt::Color(0xff6d7480), egt::Palette::GroupId::active);

        add(m_grip);
        m_grip.resize(egt::Size(50, 50));
        m_grip.align(egt::AlignFlag::right | egt::AlignFlag::bottom);

        add(m_arrows);
        m_arrows.resize(box().size() / 2);
        m_arrows.image().resize(box().size() / 2);
        m_arrows.align(egt::AlignFlag::center);
    }

    virtual void handle(egt::Event& event) override
    {
        egt::Window::handle(event);

        switch (event.id())
        {
        case egt::EventId::pointer_drag_start:
            m_start_point = box().point();
            break;
        case egt::EventId::pointer_drag:
        {
            auto diff = event.pointer().drag_start - event.pointer().point;
            move(m_start_point - egt::Point(diff.x(), diff.y()));
            break;
        }
        default:
            break;
        }
    }

protected:
    egt::ImageLabel m_grip;
    egt::ImageLabel m_arrows;
    egt::Point m_start_point;
    egt::PropertyAnimator m_animationx;
    egt::PropertyAnimator m_animationy;
};

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv, "drag");

    egt::TopWindow window;
    window.background(egt::Image("file:background.png"));

    FloatingBox box1(egt::Rect(egt::Ratio<int>(window.width(), 20),
                               egt::Ratio<int>(window.height(), 20),
                               egt::Ratio<int>(window.width(), 20),
                               egt::Ratio<int>(window.width(), 20)));
    window.add(box1);

    FloatingBox box2(egt::Rect(egt::Ratio<int>(window.width(), 20) * 3,
                               egt::Ratio<int>(window.height(), 20),
                               egt::Ratio<int>(window.width(), 20),
                               egt::Ratio<int>(window.width(), 20)));
    window.add(box2);

    box1.show();
    box2.show();

    egt::Label label1("CPU: ----", {}, egt::AlignFlag::left | egt::AlignFlag::center);
    label1.align(egt::AlignFlag::left | egt::AlignFlag::bottom);
    label1.color(egt::Palette::ColorId::label_text, egt::Palette::white);
    label1.color(egt::Palette::ColorId::bg, egt::Palette::transparent);
    window.add(label1);

    egt::ImageLabel logo(egt::Image("icon:egt_logo_white.png;128"));
    logo.margin(10);
    window.add(center(top(logo)));

    egt::experimental::CPUMonitorUsage tools;
    egt::PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&tools, &label1]()
    {
        tools.update();

        std::ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage(0)) << "%";
        label1.text(ss.str());
    });
    cputimer.start();

    window.show();

    return app.run();
}
