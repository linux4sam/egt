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

static Animation* animation = 0;

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

static vector<StringItem> curves_names =
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

class MyListBox : public ListBox
{
public:
    MyListBox(const item_array& items,
              const Rect& rect = Rect())
        : ListBox(items, rect)
    {}

    void set_select(uint32_t index) override
    {
        ListBox::set_select(index);

        animation->stop();
        animation->set_easing_func(curves[index]);
        animation->start();
    }
};

class MainWindow : public TopWindow
{
public:
    MainWindow()
    {}

    int load()
    {
        Image* img = new Image("background.png");
        double scale = (double)w() / (double)img->w();
        add(img);
        img->scale(scale, scale);

        MyListBox::item_array items;
        items.resize(curves_names.size());
        transform(curves_names.begin(), curves_names.end(), items.begin(),
                  [](const StringItem & v) { return new StringItem(v);});
        MyListBox* list1 = new MyListBox(items, Rect(Point(w() - 100, 0), Size(100, h())));
        add(list1);
        list1->set_select(7);

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

        return 0;
    }

    void move_item(int y)
    {
        m_box->move(Point(m_box->x(), y));
    }

private:
    BasicWindow* m_box{nullptr};
};

struct ResetTimer : public Timer
{
    ResetTimer()
        : Timer(std::chrono::seconds(1))
    {}

    void timeout() override
    {
        animation->start();
    }
};

struct MyAnimationTimer : public PeriodicTimer
{
    explicit MyAnimationTimer(ResetTimer& reset)
        : PeriodicTimer(std::chrono::milliseconds(30)),
          m_reset(reset)
    {}

    void timeout() override
    {
        if (animation->running())
            animation->next();
        else if (!m_reset.running())
            m_reset.start();
    }

    ResetTimer& m_reset;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    set_image_path("../share/egt/examples/easing/");

    ResetTimer reset_timer;
    MyAnimationTimer animation_timer(reset_timer);

    MainWindow window;

    animation = new Animation(-110, window.h() - 100, [&window](float value)
    {
        window.move_item(value);
    }, std::chrono::seconds(2), easing_linear);

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

    animation_timer.start();

    window.show();

    return app.run();
}
