/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cassert>
#include <chrono>
#include <cmath>
#include <egt/ui>
#include <random>
#include <vector>
#include <sstream>

using namespace egt;

class Ball : public ImageLabel
{
public:
    Ball() = delete;

    Ball(int xspeed, int yspeed) noexcept
        : ImageLabel(Image("star.png")),
          m_xspeed(xspeed),
          m_yspeed(yspeed)
    {
        flags().set(Widget::Flag::no_layout);
        flags().set(Widget::Flag::no_autoresize);
    }

    Ball(const Ball&) = default;
    Ball(Ball&&) = default;
    Ball& operator=(const Ball&) = default;
    Ball& operator=(Ball&&) = default;
    virtual ~Ball() = default;

    // returns false when it's no longer visible
    bool animate()
    {
        assert(parent());

        bool visible = box().intersect(parent()->box());

        if (visible)
        {
            Point to(point());
            to += Point(m_xspeed, m_yspeed);
            move(to);
        }

        return visible;
    }

private:

    int m_xspeed{0};
    int m_yspeed{0};
};

static bool debounce_mouse(int delta)
{
    static std::chrono::time_point<std::chrono::steady_clock> last_time =
        std::chrono::steady_clock::now();

    if (std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() -
            last_time).count() > delta)
    {
        last_time = std::chrono::steady_clock::now();
        return true;
    }

    return false;
}

class MainWindow : public TopWindow
{
public:
    MainWindow()
    {
        background(Image("background.png"));

        auto logo = std::make_shared<ImageLabel>(Image("@128px/egt_logo_white.png"));
        logo->margin(10);
        add(top(left(logo)));
    }

    void handle(Event& event) override
    {
        TopWindow::handle(event);

        switch (event.id())
        {
        case EventId::raw_pointer_move:
            if (debounce_mouse(100))
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

        // has to move at some speed!
        if (xspeed == 0 && yspeed == 0)
            xspeed = yspeed = 1;

        m_images.emplace_back(std::make_shared<Ball>(xspeed, yspeed));

        auto image = m_images.back();
        image->resize_by_ratio(scale);
        image->image_align(AlignFlag::expand);
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
    std::vector<std::shared_ptr<Ball>> m_images;

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
    label1.color(Palette::ColorId::text, Palette::white);
    label1.color(Palette::ColorId::bg, Palette::transparent);
    win.add(bottom(left(label1)));

    experimental::CPUMonitorUsage tools;
    PeriodicTimer cputimer(std::chrono::seconds(1));
    cputimer.on_timeout([&label1, &tools]()
    {
        tools.update();
        std::ostringstream ss;
        ss << "CPU: " << static_cast<int>(tools.usage(0)) << "%";
        label1.text(ss.str());
    });
    cputimer.start();

    std::vector<std::unique_ptr<AnimationSequence>> animations;

    if (argc > 1)
    {
        // create starfield effect

        std::random_device r;
        std::default_random_engine e1 {r()};
        std::uniform_int_distribution<int> x_dist {0, 800};
        std::uniform_int_distribution<int> y_dist {0, 480};
        std::uniform_int_distribution<int> d_dist {500, 4000};
        std::uniform_int_distribution<int> s_dist {10, 30};

        for (int i = 0; i < 25; i++)
        {
            auto star = std::make_shared<Ball>(0, 0);
            star->resize_by_ratio(s_dist(e1));
            star->image_align(AlignFlag::expand);
            star->move_to_center(Point(x_dist(e1), y_dist(e1)));
            win.add(star);

            auto in = std::make_shared<PropertyAnimatorF>(0, 1,
                      std::chrono::milliseconds(d_dist(e1)),
                      easing_spring);
            in->on_change([star](PropertyAnimatorF::Value value) { star->alpha(value); });

            auto out = std::make_shared<PropertyAnimatorF>(1, 0,
                       std::chrono::milliseconds(d_dist(e1)),
                       easing_spring);
            out->reverse(true);
            out->on_change([star](PropertyAnimatorF::Value value) { star->alpha(value); });

            auto delay = std::make_shared<AnimationDelay>(std::chrono::milliseconds(d_dist(e1)));

            animations.emplace_back(detail::make_unique<AnimationSequence>(true));
            auto& sequence = animations.back();
            sequence->add(delay);
            sequence->add(in);
            sequence->add(out);

            sequence->start();
        }
    }

    try
    {
        return app.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
