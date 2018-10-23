/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <chrono>
#include <cmath>
#include <mui/ui>
#include <random>
#include <vector>

using namespace std;
using namespace mui;

class Ball : public Image
{
public:
    Ball(int xspeed, int yspeed) noexcept
        : Image("metalball.png"),
          m_xspeed(xspeed),
          m_yspeed(yspeed)
    {}

    bool animate()
    {
        bool visible = Rect::intersect(parent()->box(), box());

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
};

static bool debounce_mouse(int delta)
{
    static Point pos;
    bool res = false;

    if (delta)
    {
        if (std::abs(pos.x - mouse_position().x) > delta ||
            std::abs(pos.y - mouse_position().y) > delta)
        {
            res = true;
        }
    }

    pos = mouse_position();

    return res;;
}

class MainWindow : public Window
{
public:
    MainWindow()
    {
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    }

    int handle(int event)
    {
        switch (event)
        {
        case EVT_MOUSE_MOVE:
            if (debounce_mouse(5))
                spawn(mouse_position());
            break;
        }

        return Window::handle(event);
    }

    void spawn(const Point& p)
    {
        int xspeed = speed_dist(e1);
        int yspeed = speed_dist(e1);
        float size = size_dist(e1);

        // has to move at some speed
        if (xspeed == 0 && yspeed == 0)
            xspeed = yspeed = 1;

        Ball* image = new Ball(xspeed, yspeed);
        add(image);
        image->scale(size, size, true);
        image->move_to_center(p);
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
    std::random_device r;
    std::default_random_engine e1 {r()};
    std::uniform_int_distribution<int> speed_dist {-20, 20};
    std::uniform_real_distribution<float> size_dist {0.1, 1.0};

    vector<Ball*> m_images;
};

int main()
{
    Application app;

    set_image_path("../share/mui/examples/space/");

    MainWindow win;
    win.show();

    PeriodicTimer animatetimer(std::chrono::milliseconds(30));
    animatetimer.add_handler([&win]()
    {
        win.animate();
    });
    animatetimer.start();

    PeriodicTimer spawntimer(std::chrono::seconds(1));
    spawntimer.add_handler([&win]()
    {
        win.spawn(win.box().center());
    });
    spawntimer.start();

    Label label1("CPU: 0%",
                 Rect(Point(10, win.size().h - 40),
                      Size(100, 40)));
    label1.palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE)
    .set(Palette::BG, Palette::GROUP_NORMAL, Color::TRANSPARENT);
    win.add(&label1);

    CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.add_handler([&label1, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << (int)tools.usage(0) << "%";
        label1.text(ss.str());
    });
    cputimer.start();

    return app.run();
}
