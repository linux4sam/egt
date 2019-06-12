/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <egt/ui>
#include <egt/painter.h>
#include <sstream>
#include <string>

using namespace std;
using namespace egt;

class MainWindow;

shared_ptr<MainWindow> win1;

class MyButton : public ImageButton
{
public:
    MyButton(const string& filename,
             const string& label,
             int x = 0,
             int y = 0)
        : ImageButton(Image(filename), label, Rect(Point(x, y), Size()))
    {
        set_image_align(alignmask::left);
        set_text_align(alignmask::center | alignmask::bottom);
    }
};

class HomeImage : public ImageLabel
{
public:
    HomeImage(Frame& parent, const string& filename,
              int x = 0,
              int y = 0)
        : ImageLabel(parent, Image(filename), "", Rect(Point(x, y), Size()))
    {}

};

class Box : public Widget
{
public:
    Box(Frame& parent, const Rect& rect, const Color& color)
        : Widget(parent, rect),
          m_color(color)
    {}

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        painter.set(m_color);
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
        painter.draw(rect);
        painter.fill();
    }

protected:
    Color m_color;
};

static void top_menu(Window& win)
{
    auto box1 = std::make_shared<Box>(win, Rect(Size(800, 60)), Palette::black);

    auto i1 = std::make_shared<HomeImage>(win, "home.png", 5, 5);

    auto l1 = std::make_shared<Label>(win, "", Rect(Point(320, 0), Size(100, 60)), alignmask::center);
    l1->set_color(Palette::ColorId::text, Palette::white);
    l1->set_color(Palette::ColorId::bg, Palette::transparent);

    struct TimeTimer : public PeriodicTimer
    {
        explicit TimeTimer(Label& label)
            : PeriodicTimer(std::chrono::milliseconds(100)),
              m_label(label)
        {}

        void timeout() override
        {
            auto now = std::chrono::system_clock::now();
            time_t tt = std::chrono::system_clock::to_time_t(now);
            tm local_tm;
            localtime_r(&tt, &local_tm);

            ostringstream ss;
            ss << local_tm.tm_hour << ":" << std::setfill('0') << std::setw(2) << local_tm.tm_min;
            m_label.set_text(ss.str());
        }

        Label& m_label;
    };

    auto timer = std::make_shared<TimeTimer>(*l1);

    timer->start();

    auto l2 = std::make_shared<Label>("48°", Rect(Point(420, 0), Size(100, 60)), alignmask::center);
    l2->set_color(Palette::ColorId::text, Palette::white);
    l2->set_color(Palette::ColorId::bg, Palette::transparent);
    win.add(l2);

    auto i2 = std::make_shared<ImageLabel>(Image("wifi.png"), "", Rect(Point(800 - 50, 10), Size()));
    win.add(i2);
}

static void bottom_menu(Window& win)
{
    auto grid2 = std::make_shared<StaticGrid>(win, Rect(Point(0, 390), Size(800, 90)), Tuple(5, 1), 4);
    grid2->set_color(Palette::ColorId::border, Palette::transparent);

    auto bb1 = std::make_shared<MyButton>("audio_s.png", _("Audio"), 0, 0);
    grid2->add(expand(bb1), 0, 0);

    auto bb2 = std::make_shared<MyButton>("climate_s.png", _("Climate"), 0, 0);
    grid2->add(expand(bb2), 1, 0);

    auto bb3 = std::make_shared<MyButton>("navigation_s.png", _("Nav"), 0, 0);
    grid2->add(expand(bb3), 2, 0);

    auto bb4 = std::make_shared<MyButton>("phone_s.png", _("Phone"), 0, 0);
    grid2->add(expand(bb4), 3, 0);

    auto bb5 = std::make_shared<MyButton>("apps_s.png", _("Apps"), 0, 0);
    grid2->add(expand(bb5), 4, 0);
}

class MainWindow : public TopWindow
{
public:
    explicit MainWindow(const Size& size)
        : TopWindow(size),
          grid(Rect(Point(0, 60), Size(800, 330)), Tuple(4, 2), 10)
    {
        set_color(Palette::ColorId::bg, Palette::lightblue);

        grid.set_color(Palette::ColorId::border, Palette::transparent);

        add(grid);

        auto b1 = std::make_shared<MyButton>("sound.png", _("Sound"));
        grid.add(expand(b1));

        auto b2 = std::make_shared<MyButton>("clock.png", _("Clock"));
        grid.add(expand(b2));

        auto b3 = std::make_shared<MyButton>("bluetooth.png", _("Bluetooth"));
        grid.add(expand(b3));

        auto b4 = std::make_shared<MyButton>("phone.png", _("Phone"));
        grid.add(expand(b4));

        auto b5 = std::make_shared<MyButton>("apps.png", _("Mobile Apps"));
        grid.add(expand(b5));

        auto b6 = std::make_shared<MyButton>("navigation.png", _("Navigation"));
        grid.add(expand(b6));

        auto b7 = std::make_shared<MyButton>("general.png", _("General"));
        grid.add(expand(b7));

        top_menu(*this);
        bottom_menu(*this);
    }

    StaticGrid grid;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "infotain");

    win1 = make_shared<MainWindow>(Size(800, 480));

    win1->show();

    return app.run();
}
