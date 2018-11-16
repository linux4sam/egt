/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <chrono>
#include <cmath>
#include <egt/ui>
#include <random>
#include <vector>

using namespace std;
using namespace egt;

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
    static std::chrono::time_point<std::chrono::steady_clock> last_time =
        chrono::steady_clock::now();


    if (chrono::duration<float_t, milli>(chrono::steady_clock::now() - last_time).count() > delta)
    {
        last_time =
            chrono::steady_clock::now();
        return true;
    }

    return false;

#if 0
    static Point pos;
    bool res = false;

    if (delta)
    {
        if (std::abs(pos.x - event_mouse().x) > delta ||
            std::abs(pos.y - event_mouse().y) > delta)
        {
            res = true;
        }
    }

    pos = event_mouse();

    return res;
#endif
}

class MainWindow : public Window
{
public:
    MainWindow()
    {
        palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    }

    int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::MOUSE_MOVE:
            if (debounce_mouse(50))
                spawn(event_mouse());
            break;
        default:
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

    set_image_path("../share/egt/examples/space/");

    MainWindow win;
    win.show();

    PeriodicTimer animatetimer(std::chrono::milliseconds(30));
    animatetimer.on_timeout([&win]()
    {
        win.animate();
    });
    animatetimer.start();

    PeriodicTimer spawntimer(std::chrono::seconds(1));
    spawntimer.on_timeout([&win]()
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
    cputimer.on_timeout([&label1, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << (int)tools.usage(0) << "%";
        label1.set_text(ss.str());
    });
    cputimer.start();

    return app.run();
}
