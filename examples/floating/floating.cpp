/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/ui>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

class FloatingBox
{
public:
    FloatingBox(shared_ptr<Widget> widget, DefaultDim mx, DefaultDim my)
        : m_widget(std::move(widget)),
          m_mx(mx),
          m_my(my)
    {
        m_widget->flags().set(Widget::Flag::no_layout);
        m_widget->flags().set(Widget::Flag::grab_mouse);
        widget->on_event(std::bind(&FloatingBox::handle, this, std::placeholders::_1));
    }

    virtual void handle(Event& event)
    {
        switch (event.id())
        {
        case EventId::pointer_dblclick:
            m_mx *= -1;
            m_my *= -1;
            break;
        case EventId::pointer_drag_start:
            m_start_point = m_widget->box().point();
            m_dragging = true;
            break;
        case EventId::pointer_drag_stop:
            m_dragging = false;
            break;
        case EventId::pointer_drag:
        {
            auto diff = event.pointer().drag_start - event.pointer().point;
            auto fromstart = m_start_point - Point(diff.x(), diff.y());
            Rect dest(fromstart, m_widget->box().size());
            if (main_window()->box().contains(dest))
                m_widget->move(fromstart);
            break;
        }
        default:
            break;
        }
    }

    virtual void next_frame()
    {
        if (m_dragging)
            return;

        auto p = Point(m_widget->x() + m_mx,
                       m_widget->y() + m_my);

        if (m_widget->box().right() >= main_window()->size().width())
            m_mx = std::fabs(m_mx) * -1;

        if (p.x() < 0)
            m_mx = std::fabs(m_mx);

        if (m_widget->box().bottom() >= main_window()->size().height())
            m_my = std::fabs(m_my) * -1;

        if (p.y() < 0)
            m_my = std::fabs(m_my);

        m_widget->move(p);
    }

    virtual ~FloatingBox() = default;

protected:
    shared_ptr<Widget> m_widget;
    DefaultDim m_mx;
    DefaultDim m_my;
    Point m_start_point;
    bool m_dragging{false};
};

static vector<shared_ptr<FloatingBox>> boxes;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "floating");

    TopWindow win;
    win.background(Image("background.png"));
    win.show();

    DefaultDim f = 2;

    vector<std::pair<DefaultDim, DefaultDim>> moveparms =
    {
        std::make_pair(1 * f, 2 * f),
        std::make_pair(3 * f, -2 * f),
        std::make_pair(-3 * f, 2 * f),
        std::make_pair(-3 * f, 3 * f),
        std::make_pair(2 * f, 3 * f),
        std::make_pair(2 * f, 2 * f),
        std::make_pair(4 * f, 2 * f),
        std::make_pair(-4 * f, 2 * f),
    };

    auto image_index = 0;

    const auto SOFT_COUNT = 2;

    // software
    for (auto x = 0; x < SOFT_COUNT; x++)
    {
        stringstream os;
        os << "image" << image_index++ << ".png";
        auto image = make_shared<ImageLabel>(Image(os.str()));
        boxes.push_back(make_shared<FloatingBox>(image, moveparms[x].first, moveparms[x].second));
        win.add(image);
    }

    const auto SOFTHARD_COUNT = 2;

    // hardware (or emulated)
    for (auto x = 0; x < SOFTHARD_COUNT; x++)
    {
        stringstream os;
        os << "image" << image_index++ << ".png";
        auto image = make_shared<ImageLabel>(Image(os.str()));
        auto plane = make_shared<Window>(Size(image->width(), image->height()));
        plane->color(Palette::ColorId::bg, Palette::transparent);
        plane->boxtype().clear();
        plane->add(image);
        plane->show();
        plane->move(Point(100, 100));
        boxes.push_back(make_shared<FloatingBox>(plane, moveparms[x].first, moveparms[x].second));
        win.add(plane);
    }

    PeriodicTimer movetimer(std::chrono::milliseconds(30));
    movetimer.on_timeout([&]()
    {
        for (auto& i : boxes)
            i->next_frame();
    });
    movetimer.start();

    Label label1("CPU: ----");
    label1.color(Palette::ColorId::text, Palette::white);
    win.add(bottom(left(label1)));

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

    return app.run();
}
