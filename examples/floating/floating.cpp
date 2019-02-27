/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/ui>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

class MyWindow : public TopWindow
{
public:
    MyWindow()
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
    }

    ImageLabel m_img;
};

class FloatingBox
{
public:
    FloatingBox(shared_ptr<Widget> widget, default_dim_type mx, default_dim_type my)
        : m_widget(widget),
          m_mx(mx),
          m_my(my)
    {
        m_widget->flags().set(Widget::flag::grab_mouse);
        widget->on_event(std::bind(&FloatingBox::handle, this, std::placeholders::_1));
    }

    virtual int handle(eventid event)
    {
        switch (event)
        {
        case eventid::pointer_dblclick:
            m_mx *= -1;
            m_my *= -1;
            break;
        case eventid::pointer_drag_start:
            m_start_point = m_widget->box().point();
            m_dragging = true;
            break;
        case eventid::pointer_drag_stop:
            m_dragging = false;
            break;
        case eventid::pointer_drag:
        {
            auto diff = (event::pointer().drag_start - event::pointer().point);
            auto fromstart = m_start_point - Point(diff.x, diff.y);
            Rect dest(fromstart, m_widget->box().size());
            if (main_window()->box().contains(dest))
                m_widget->move(fromstart);
            break;
        }
        default:
            break;
        }

        return 0;
    }

    virtual void next_frame()
    {
        if (m_dragging)
            return;

        auto p = Point(m_widget->x() + m_mx,
                       m_widget->y() + m_my);

        if (m_widget->box().right() >= main_window()->size().w)
            m_mx = std::fabs(m_mx) * -1;

        if (p.x < 0)
            m_mx = std::fabs(m_mx);

        if (m_widget->box().bottom() >= main_window()->size().h)
            m_my = std::fabs(m_my) * -1;

        if (p.y < 0)
            m_my = std::fabs(m_my);

        m_widget->move(p);
    }

protected:
    shared_ptr<Widget> m_widget;
    default_dim_type m_mx;
    default_dim_type m_my;
    Point m_start_point;
    bool m_dragging{false};
};

static vector<shared_ptr<FloatingBox>> boxes;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "floating");

    MyWindow win;
    win.show();

    default_dim_type f = 2;

    vector<std::pair<default_dim_type, default_dim_type>> moveparms =
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
        auto plane = make_shared<Window>(Size(image->w(), image->h()));
        plane->palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::transparent);
        plane->set_boxtype(Theme::boxtype::none);
        plane->add(image);
        plane->show();
        plane->move(Point(100, 100));
        boxes.push_back(make_shared<FloatingBox>(plane, moveparms[x].first, moveparms[x].second));
        win.add(plane);
    }

    PeriodicTimer movetimer(std::chrono::milliseconds(30));
    movetimer.on_timeout([&]()
    {
        for (auto i : boxes)
            i->next_frame();
    });
    movetimer.start();

    Label label1("CPU: -",
                 Rect(Point(10, win.size().h - 40),
                      Size(100, 40)),
                 alignmask::left | alignmask::center);
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

    return app.run();
}
