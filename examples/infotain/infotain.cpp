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
class ChildWindow;

MainWindow* win1;
ChildWindow* win2;

class MyButton : public ImageButton
{
public:
    MyButton(const string& filename,
             const string& label,
             int x = 0,
             int y = 0,
             widgetmask flags = widgetmask::NONE)
        : ImageButton(Image(filename), label, Rect(Point(x, y), Size()), flags)
    {
        image_align(alignmask::CENTER);
        set_text_align(alignmask::CENTER | alignmask::BOTTOM);
    }

    int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            if (main_window() == reinterpret_cast<Window*>(win1))
            {
                main_window()->exit();
                main_window() = reinterpret_cast<Window*>(win2);
                main_window()->enter();
            }
            else
            {
                main_window()->exit();
                main_window() = reinterpret_cast<Window*>(win1);
                main_window()->enter();
            }
            break;
        }
        case eventid::MOUSE_UP:
            break;
        default:
            break;
        }

        return ImageButton::handle(event);
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

    int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::MOUSE_DOWN:
        {
            if (main_window() == reinterpret_cast<Window*>(win1))
            {
                main_window()->exit();
                main_window() = reinterpret_cast<Window*>(win2);
                main_window()->enter();
            }
            else
            {
                main_window()->exit();
                main_window() = reinterpret_cast<Window*>(win1);
                main_window()->enter();
            }
            break;
        }
        case eventid::MOUSE_UP:
            break;
        default:
            break;
        }

        return ImageLabel::handle(event);
    }
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

static void top_menu(BasicWindow* win)
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

static void bottom_menu(BasicWindow* win)
{
    Box* box2 = new Box(Rect(Point(0, 390), Size(800, 90)), Color::LIGHTGRAY);
    win->add(box2);

    StaticGrid* grid2 = new StaticGrid(Rect(Point(0, 390), Size(800, 90)), 5, 1, 0);
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

    grid2->reposition();
}

class MainWindow : public TopWindow
{
public:
    explicit MainWindow(const Size& size)
        : TopWindow(size),
          grid(Rect(Point(0, 60), Size(800, 330)), 4, 2, 10)
    {
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::LIGHTBLUE);

        add(&grid);

        MyButton* b1 = new MyButton("sound.png", _("Sound"));
        grid.add(b1, 0, 0);

        MyButton* b2 = new MyButton("clock.png", _("Clock"));
        grid.add(b2, 1, 0);

        MyButton* b3 = new MyButton("bluetooth.png", _("Bluetooth"));
        grid.add(b3, 2, 0);

        MyButton* b4 = new MyButton("phone.png", _("Phone"));
        grid.add(b4, 3, 0);

        MyButton* b5 = new MyButton("apps.png", _("Mobile Apps"));
        grid.add(b5, 0, 1);

        MyButton* b6 = new MyButton("navigation.png", _("Navigation"));
        grid.add(b6, 1, 1);

        MyButton* b7 = new MyButton("general.png", _("General"));
        grid.add(b7, 2, 1);

        grid.reposition();

        top_menu(this);
        bottom_menu(this);
    }

    StaticGrid grid;
};

class ChildWindow : public Window
{
public:
    explicit ChildWindow(const Size& size)
        : Window(size)
    {
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::LIGHTBLUE);

        Label* label = new Label(_("Blank Screen"), Rect(Point(0, 60), Size(800, 330)));
        add(label);

        top_menu(this);
        bottom_menu(this);
    }
};

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    set_image_path("../share/egt/examples/infotain/");

    win1 = new MainWindow(Size(800, 480));
    win2 = new ChildWindow(Size(800, 480));

    /** @todo Broken. enter()/leave() not fully implemented. */
    //win1->add(win2);

    win1->show();

    return app.run();
}
