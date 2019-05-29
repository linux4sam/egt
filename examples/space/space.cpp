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
    {
        flags().set(Widget::flag::no_layout);
    }

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
            Point to(point());
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


    if (chrono::duration<float, milli>(chrono::steady_clock::now() - last_time).count() > delta)
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
        set_color(Palette::ColorId::bg, Palette::black);
    }

    void handle(Event& event) override
    {
        TopWindow::handle(event);

        switch (event.id())
        {
        case eventid::raw_pointer_move:
            if (debounce_mouse(50))
                spawn(display_to_local(event.pointer().point));
            break;
        default:
            break;
        }
    }

    void spawn(const Point& p)
    {
        auto xspeed = speed_dist(e1);
        auto yspeed = speed_dist(e1);
        auto scale = size_dist(e1);

        // has to move at some speed
        if (xspeed == 0 && yspeed == 0)
            xspeed = yspeed = 1;

        m_images.emplace_back(make_shared<Ball>(xspeed, yspeed));
        auto image = m_images.back();
        image->resize_by_ratio(scale);
        image->set_image_align(alignmask::expand);
        image->move_to_center(p);
        add(image);
    }

    void animate()
    {
        for (auto i = m_images.begin(); i != m_images.end();)
        {
            auto& image = *i;
            if (!image->animate())
            {
                image->detach();
                i = m_images.erase(i);
            }
            else
            {
                ++i;
            }
        }
    }

private:
    vector<shared_ptr<Ball>> m_images;

    std::random_device r;
    std::default_random_engine e1 {r()};
    std::uniform_int_distribution<int> speed_dist {-20, 20};
    std::uniform_int_distribution<int> size_dist {10, 100};
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
        win.spawn(win.center());
    });
    spawntimer.start();

    Label label1("CPU: ----");
    label1.set_color(Palette::ColorId::text, Palette::white);
    label1.set_color(Palette::ColorId::bg, Palette::transparent);
    win.add(bottom(left(label1)));

    CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&label1, &tools]()
    {
        tools.update();
        ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage(0)) << "%";
        label1.set_text(ss.str());
    });
    cputimer.start();

    std::random_device r;
    std::default_random_engine e1 {r()};
    std::uniform_int_distribution<int> x_dist {0, 800};
    std::uniform_int_distribution<int> y_dist {0, 480};
    std::uniform_int_distribution<int> d_dist {100, 5000};
    std::uniform_int_distribution<int> s_dist {2, 6};

    for (int i = 0; i < 25; i++)
    {
        auto star = make_shared<CircleWidget>(Circle(Point(x_dist(e1), y_dist(e1)), s_dist(e1)));
        win.add(star);
        star->set_color(Palette::ColorId::button_bg, Palette::white);

        auto in = new PropertyAnimator(0, 255, std::chrono::seconds(3), easing_spring);
        in->on_change([star](float_t value)
        {
            auto color = star->color(Palette::ColorId::button_bg);
            color.color().alpha(value);
            star->set_color(Palette::ColorId::button_bg, color);
        });

        auto out = new PropertyAnimator(255, 0, std::chrono::seconds(3), easing_spring);
        out->reverse(true);
        out->on_change([star](float_t value)
        {
            auto color = star->color(Palette::ColorId::button_bg);
            color.color().alpha(value);
            star->set_color(Palette::ColorId::button_bg, color);
        });

        auto delay = new AnimationDelay(std::chrono::milliseconds(d_dist(e1)));

        auto sequence = new AnimationSequence(true);
        sequence->add(*delay);
        sequence->add(*in);
        sequence->add(*out);

        sequence->start();
    }

    try
    {
        return app.run();
    }
    catch (std::exception& e)
    {
        cerr << e.what() << endl;
    }

    return 1;
}
