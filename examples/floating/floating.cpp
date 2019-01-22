/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/ui>
#include <egt/detail/mousegesture.h>
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
        : TopWindow(Size(), widgetmask::NO_BACKGROUND),
          m_img(Image("background.png"))
    {
        add(&m_img);
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
    FloatingBox(Widget* widget, int mx, int my)
        : m_widget(widget),
          m_mx(mx),
          m_my(my)
    {
        m_widget->flag_set(widgetmask::GRAB_MOUSE);

        widget->on_event([this](eventid event)
        {
            auto mouse = m_mouse.handle(event);
            switch (mouse)
            {
            case Swipe::mouse_event::done:
            case Swipe::mouse_event::none:
                break;
            case Swipe::mouse_event::start:
                m_mouse.start(m_widget->box().point());
                return 1;
            case Swipe::mouse_event::drag:
            {
                auto diff = m_mouse.start_value() -
                            (m_mouse.mouse_start() - event_mouse());
                Rect dest(diff, m_widget->box().size());
                if (main_window()->box().contains(dest))
                    m_widget->move(diff);
                return 1;
            }
            case Swipe::mouse_event::click:
                break;
            }

            return 0;
        });
    }

    virtual void next_frame()
    {
        if (m_mouse.dragging())
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
    Widget* m_widget;
    int m_mx;
    int m_my;

    using Swipe = detail::MouseGesture<Point>;

    Swipe m_mouse;
};

static vector<FloatingBox*> boxes;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "floating");

    MyWindow win;
    win.show();

    int f = 2;

    vector<std::pair<int, int>> moveparms =
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

#ifdef HAVE_LIBPLANES
    uint32_t SOFT_COUNT = 2;
#else
    uint32_t SOFT_COUNT = 4;
#endif

    // software
    for (uint32_t x = 0; x < SOFT_COUNT; x++)
    {
        stringstream os;
        os << "image" << x << ".png";
        auto image = new ImageLabel(Image(os.str()));
        image->set_name("software " + os.str());
        boxes.push_back(new FloatingBox(image, moveparms[x].first, moveparms[x].second));
        win.add(image);
    }

#ifdef HAVE_LIBPLANES
    int total = KMSScreen::instance()->count_planes(KMSScreen::plane_type::overlay);
#endif

#ifdef HAVE_LIBPLANES
    // hardware (or emulated)
    for (uint32_t x = SOFT_COUNT; x < SOFT_COUNT + total; x++)
    {
        stringstream os;
        os << "image" << x << ".png";
        auto image = new ImageLabel(Image(os.str()));
        image->set_name("hardware " + os.str());
        auto plane = new Window(Size(image->w(), image->h()));
        plane->palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        plane->flag_set(widgetmask::NO_BACKGROUND);
        plane->add(image);
        plane->show();
        plane->move(Point(100, 100));
        boxes.push_back(new FloatingBox(plane, moveparms[x].first, moveparms[x].second));
        win.add(plane);
    }
#endif

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
