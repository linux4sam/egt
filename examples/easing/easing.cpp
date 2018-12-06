/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <egt/ui>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <random>
#include <cmath>

using namespace std;
using namespace egt;

#ifdef HAVE_LIBPLANES
#define USE_HARDWARE
#endif

static easing_func curves[] =
{
    easing_linear,
    easing_easy,
    easing_easy_slow,
    easing_extend,
    easing_drop,
    easing_drop_slow,
    easing_snap,
    easing_bounce,
    easing_bouncy,
    easing_rubber,
    easing_spring,
    easing_boing
};

static vector<StringItem> curve_names =
{
    "linear",
    "easy",
    "easy slow",
    "extend",
    "drop",
    "drop slow",
    "snap",
    "bounce",
    "bouncy",
    "rubber",
    "spring",
    "boing"
};

class MainWindow : public TopWindow
{
public:
    MainWindow()
        : TopWindow(),
          m_seq(true),
          m_animation(-110, h() - 100, std::chrono::seconds(2)),
          m_delay(std::chrono::seconds(1))
    {}

    int load()
    {
        Image* img = new Image("background.png");
        double scale = (double)w() / (double)img->w();
        add(img);
        img->scale(scale, scale);

        ListBox::item_array items;
        items.resize(curve_names.size());
        transform(curve_names.begin(), curve_names.end(), items.begin(),
        [](const StringItem & v) { return new StringItem(v);});
        ListBox* list1 = new ListBox(items, Rect(Point(w() - 100, 0), Size(100, h())));
        add(list1);

        list1->on_event([this, list1](eventid event)
        {
            if (event == eventid::PROPERTY_CHANGED)
            {
                m_seq.reset();
                m_animation.set_easing_func(curves[list1->selected()]);
                m_seq.start();
            }

            return 0;
        });

        Image* image = new Image("ball.png");
        // There is a bug on 9x5 that if the plane is all the way out of view
        // then it will cause glitches. So, we create the height (which will
        // be invisible), to always keep a portion of the plane on screen
        // alternate of making the plane the same exact size as the image.
#ifdef USE_HARDWARE
        m_box = new Window(Size(100, 200));
#else
        m_box = new BasicWindow(Size(100, 200));
#endif
        //m_box->palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
        m_box->flag_set(widgetmask::NO_BACKGROUND);
        m_box->add(image);
        m_box->move(Point(w() / 2 - m_box->w() / 2, -110));
        m_box->show();

        add(m_box);

        m_animation.on_change(std::bind(&BasicWindow::set_y, m_box, std::placeholders::_1));
        m_seq.add(&m_animation);
        m_seq.add(&m_delay);

        list1->set_select(7);

        m_seq.start();

        return 0;
    }

private:
    BasicWindow* m_box{nullptr};

    AnimationSequence m_seq;
    PropertyAnimator m_animation;
    AnimationDelay m_delay;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    set_image_path("../share/egt/examples/easing/");

    MainWindow window;

    window.load();

    Label label1("CPU: 0%",
                 Rect(Point(10, window.size().h - 40),
                      Size(100, 40)));
    label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
    window.add(&label1);

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

    window.show();

    return app.run();
}
