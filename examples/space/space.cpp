/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <chrono>
#include <cmath>
#include <egt/ui>
#include <random>
#include <vector>

using namespace std;
using namespace egt;

class Ball : public ImageLabel
{
public:
    Ball(int xspeed, int yspeed) noexcept
        : ImageLabel(Image("metalball.png")),
          m_xspeed(xspeed),
          m_yspeed(yspeed)
    {}

    Ball(const Ball&) = default;
    Ball(Ball&&) = default;
    Ball& operator=(const Ball&) = default;
    Ball& operator=(Ball&&) = default;
    virtual ~Ball() = default;

    bool animate()
    {
        assert(parent());

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

    Ball() = delete;

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

class MainWindow : public TopWindow
{
public:
    MainWindow()
    {
        palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::black);
    }

    int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::raw_pointer_move:
            if (debounce_mouse(50))
                spawn(from_display(event::pointer().point));
            break;
        default:
            break;
        }

        return TopWindow::handle(event);
    }

    void spawn(const Point& p)
    {
        auto xspeed = speed_dist(e1);
        auto yspeed = speed_dist(e1);
        auto scale = size_dist(e1);

        // has to move at some speed
        if (xspeed == 0 && yspeed == 0)
            xspeed = yspeed = 1;

        m_images.emplace_back(xspeed, yspeed);
        auto& image = m_images.back();
        image.scale_image(scale, true);
        image.move_to_center(p);
        add(image);
    }

    void animate()
    {
        for (auto i = m_images.begin(); i != m_images.end();)
        {
            auto& image = *i;
            if (!image.animate())
            {
                i = m_images.erase(i);
            }
            else
            {
                ++i;
            }
        }
    }

private:
    vector<Ball> m_images;

    std::random_device r;
    std::default_random_engine e1 {r()};
    std::uniform_int_distribution<int> speed_dist {-20, 20};
    std::uniform_real_distribution<float> size_dist {0.1, 1.0};
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "space");

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
    label1.palette().set(Palette::ColorId::text, Palette::GroupId::normal, Palette::white)
    .set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::transparent);
    win.add(label1);

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
