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

MainWindow* win1;

class MyButton : public ImageButton
{
public:
    MyButton(const string& filename,
             const string& label,
             int x = 0,
             int y = 0,
             widgetflags flags = widgetflags())
        : ImageButton(Image(filename), label, Rect(Point(x, y), Size()), flags)
    {
        set_position_image_first(true);
        set_image_align(alignmask::CENTER);
        set_text_align(alignmask::CENTER | alignmask::BOTTOM);
    }
};

class HomeImage : public ImageLabel
{
public:
    HomeImage(const string& filename,
              int x = 0,
              int y = 0)
        : ImageLabel(Image(filename), Point(x, y))
    {}

};

class Box : public Widget
{
public:
    Box(const Rect& rect, const Color& color)
        : Widget(rect),
          m_color(color)

    {}

    virtual void draw(Painter& painter, const Rect& rect) override
    {
        painter.set_color(m_color);
        cairo_set_operator(painter.context().get(), CAIRO_OPERATOR_SOURCE);
        painter.draw_fill(rect);
    }

protected:
    Color m_color;
};

static void top_menu(Window* win)
{
    Box* box1 = new Box(Rect(Size(800, 60)), Color::BLACK);
    win->add(box1);

    HomeImage* i1 = new HomeImage("home.png", 5, 5);
    win->add(i1);

    Label* l1 = new Label("", Rect(Point(320, 0), Size(100, 60)), alignmask::CENTER, Font(32));
    l1->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
    win->add(l1);

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
            tm local_tm = *localtime(&tt);

            ostringstream ss;
            ss << local_tm.tm_hour << ":" << std::setfill('0') << std::setw(2) << local_tm.tm_min;
            m_label.set_text(ss.str());
        }

        Label& m_label;
    };

    TimeTimer* timer = new TimeTimer(*l1);

    timer->start();

    Label* l2 = new Label("48°", Rect(Point(420, 0), Size(100, 60)), alignmask::CENTER, Font(24));
    l2->palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
    win->add(l2);

    auto i2 = new ImageLabel(Image("wifi.png"), Point(800 - 50, 10));
    win->add(i2);
}

static void bottom_menu(Window* win)
{
    StaticGrid* grid2 = new StaticGrid(Rect(Point(0, 390), Size(800, 90)), 5, 1, 4);
    grid2->palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);

    win->add(grid2);

    MyButton* bb1 = new MyButton("audio_s.png", _("Audio"), 0, 0);
    grid2->add(bb1, 0, 0);

    MyButton* bb2 = new MyButton("climate_s.png", _("Climate"), 0, 0);
    grid2->add(bb2, 1, 0);

    MyButton* bb3 = new MyButton("navigation_s.png", _("Nav"), 0, 0);
    grid2->add(bb3, 2, 0);

    MyButton* bb4 = new MyButton("phone_s.png", _("Phone"), 0, 0);
    grid2->add(bb4, 3, 0);

    MyButton* bb5 = new MyButton("apps_s.png", _("Apps"), 0, 0);
    grid2->add(bb5, 4, 0);
}

class MainWindow : public TopWindow
{
public:
    explicit MainWindow(const Size& size)
        : TopWindow(size),
          grid(Rect(Point(0, 60), Size(800, 330)), 4, 2, 10)
    {
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::LIGHTBLUE);

        grid.palette().set(Palette::BORDER, Palette::GROUP_NORMAL, Color::TRANSPARENT);

        add(&grid);

        MyButton* b1 = new MyButton("sound.png", _("Sound"));
        grid.add(b1);

        MyButton* b2 = new MyButton("clock.png", _("Clock"));
        grid.add(b2);

        MyButton* b3 = new MyButton("bluetooth.png", _("Bluetooth"));
        grid.add(b3);

        MyButton* b4 = new MyButton("phone.png", _("Phone"));
        grid.add(b4);

        MyButton* b5 = new MyButton("apps.png", _("Mobile Apps"));
        grid.add(b5);

        MyButton* b6 = new MyButton("navigation.png", _("Navigation"));
        grid.add(b6);

        MyButton* b7 = new MyButton("general.png", _("General"));
        grid.add(b7);

        top_menu(this);
        bottom_menu(this);
    }

    StaticGrid grid;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "infotain");

    win1 = new MainWindow(Size(800, 480));

    win1->show();

    return app.run();
}
