/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <chrono>
#include <cmath>
#include <iostream>
#include <mui/ui.h>
#include <random>
#include <string>
#include <vector>

using namespace std;
using namespace mui;

class Ball : public Image
{
public:
    Ball(int xspeed, int yspeed, const Point& point)
        : Image("metalball.png", point),
          m_xspeed(xspeed),
          m_yspeed(yspeed)
    {}

    bool animate()
    {
        bool visible = Rect::is_intersect(parent()->box(), box());

        if (visible)
        {
            Point to(box().point());
            to += Point(m_xspeed, m_yspeed);
            move(to);
        }

        return visible;
    }

private:
    int m_xspeed;
    int m_yspeed;
    float m_angle;
};

class MainWindow : public Window
{
public:
    MainWindow()
        : Window()
    {
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    }

    int handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_MOVE:
            spawn(mouse_position());
            break;
        }

        return Window::handle(event);
    }

    void spawn(const Point& p)
    {
        std::default_random_engine e1(r());
        std::uniform_int_distribution<int> speed_dist(-20, 20);
        int xspeed = speed_dist(e1);
        int yspeed = speed_dist(e1);

        std::uniform_real_distribution<float> size_dist(0.1, 1.0);
        float size = size_dist(e1);

        // has to move at some speed
        if (xspeed == 0 && yspeed == 0)
            xspeed = yspeed = 1;

        Ball* image = new Ball(xspeed, yspeed, p);
        add(image);
        image->scale(size, size);
        image->move(Point(p.x - image->box().w / 2,
                          p.y - image->box().h / 2));
        m_images.push_back(image);
    }

    void animate()
    {
        for (auto x = m_images.begin(); x != m_images.end();)
        {
            if (!(*x)->animate())
            {
                remove(*x);
                delete *x;
                x = m_images.erase(x);
            }
            else
            {
                x++;
            }
        }
    }

private:
    vector<Ball*> m_images;
    std::random_device r;
};

int main()
{
    Application app;

    set_image_path("/root/mui/share/mui/examples/space/");

    MainWindow win;
    win.show();

    PeriodicTimer animatetimer(33);
    animatetimer.add_handler([&win]()
    {
        win.animate();
    });
    animatetimer.start();

    PeriodicTimer spawntimer(1000);
    spawntimer.add_handler([&win]()
    {
        win.spawn(win.box().center());
    });
    spawntimer.start();

    return app.run();
}
