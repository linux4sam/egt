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
    easing_boing,
    easing_quadratic_easeinout,
    easing_cubic_easeinout,
    easing_quartic_easeinout,
    easing_quintic_easeinout,
    easing_sine_easeinout,
    easing_circular_easeinout,
    easing_exponential_easeinout,
};

static vector<std::string> curve_names =
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
    "boing",
    "quadratic",
    "cubic",
    "quartic",
    "quintic",
    "sine",
    "circular",
    "exponential",
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
        auto i = Image("background.png");
        auto img = new ImageLabel(i);
        double scale = (double)w() / (double)i.size().w;
        add(img);
        img->scale_image(scale);

        ListBox::item_array items;
        items.resize(curve_names.size());
        transform(curve_names.begin(), curve_names.end(), items.begin(),
        [](const std::string & v) { return new StringItem(v);});
        ListBox* list1 = new ListBox(items, Rect(Point(0, 0), Size(150, 0)));
        list1->set_align(alignmask::EXPAND_VERTICAL | alignmask::RIGHT);
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

        auto image = new ImageLabel(Image("ball.png"));
        // There is a bug on 9x5 that if the plane is all the way out of view
        // then it will cause glitches. So, we create the height (which will
        // be invisible), to always keep a portion of the plane on screen
        // alternate of making the plane the same exact size as the image.
        m_box = new Window(Size(100, 200));
        m_box->set_boxtype(Theme::boxtype::none);
        m_box->add(image);
        m_box->move(Point(w() / 2 - m_box->w() / 2, -110));
        m_box->show();

        add(m_box);

        m_animation.on_change(std::bind(&Window::set_y, m_box, std::placeholders::_1));
        m_seq.add(&m_animation);
        m_seq.add(&m_delay);

        list1->set_select(7);

        m_seq.start();

        return 0;
    }

private:
    Window* m_box{nullptr};

    AnimationSequence m_seq;
    PropertyAnimator m_animation;
    AnimationDelay m_delay;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "easing");

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
